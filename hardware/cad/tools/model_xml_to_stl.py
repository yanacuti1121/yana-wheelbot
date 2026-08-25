#!/usr/bin/env python3
"""Convert one mesh-bearing 3MF .model XML file to a binary STL."""

from __future__ import annotations

import argparse
import math
import struct
import xml.etree.ElementTree as ET
from pathlib import Path


CORE_NS = "http://schemas.microsoft.com/3dmanufacturing/core/2015/02"


def normal(a: tuple[float, float, float], b: tuple[float, float, float], c: tuple[float, float, float]) -> tuple[float, float, float]:
    ux, uy, uz = (b[i] - a[i] for i in range(3))
    vx, vy, vz = (c[i] - a[i] for i in range(3))
    nx, ny, nz = uy * vz - uz * vy, uz * vx - ux * vz, ux * vy - uy * vx
    length = math.sqrt(nx * nx + ny * ny + nz * nz)
    if length == 0:
        return 0.0, 0.0, 0.0
    return nx / length, ny / length, nz / length


def convert(source: Path, destination: Path) -> None:
    root = ET.parse(source).getroot()
    ns = {"m": CORE_NS}
    vertices = [
        tuple(float(vertex.attrib[axis]) for axis in ("x", "y", "z"))
        for vertex in root.findall(".//m:vertex", ns)
    ]
    triangles = [
        tuple(int(triangle.attrib[index]) for index in ("v1", "v2", "v3"))
        for triangle in root.findall(".//m:triangle", ns)
    ]
    if not vertices or not triangles:
        raise ValueError(f"No mesh found in {source}")

    destination.parent.mkdir(parents=True, exist_ok=True)
    with destination.open("wb") as output:
        header = f"Converted from {source.name}".encode("ascii", "replace")[:80]
        output.write(header.ljust(80, b"\0"))
        output.write(struct.pack("<I", len(triangles)))
        for triangle in triangles:
            a, b, c = (vertices[index] for index in triangle)
            output.write(struct.pack("<12fH", *normal(a, b, c), *a, *b, *c, 0))

    bounds = [
        (min(vertex[axis] for vertex in vertices), max(vertex[axis] for vertex in vertices))
        for axis in range(3)
    ]
    size = tuple(high - low for low, high in bounds)
    print(
        f"Wrote {destination}: {len(vertices)} vertices, {len(triangles)} triangles, "
        f"size {size[0]:.3f} x {size[1]:.3f} x {size[2]:.3f} mm"
    )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    args = parser.parse_args()
    convert(args.source, args.destination)


if __name__ == "__main__":
    main()
