#!/usr/bin/env python3
"""Add the Yana HC-SR04 housing parts as a new plate in a Bambu 3MF."""

from __future__ import annotations

import argparse
import html
import re
import uuid
import zipfile
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Mesh:
    vertices: tuple[tuple[float, float, float], ...]
    triangles: tuple[tuple[int, int, int], ...]


@dataclass(frozen=True)
class Part:
    name: str
    mesh: Mesh
    inner_id: int
    wrapper_id: int
    object_filename: str
    extruder: int
    x: float
    y: float


def read_ascii_stl(path: Path) -> Mesh:
    coordinates = re.findall(
        r"^\s*vertex\s+([+\-\d.eE]+)\s+([+\-\d.eE]+)\s+([+\-\d.eE]+)\s*$",
        path.read_text(encoding="ascii"),
        flags=re.MULTILINE,
    )
    if not coordinates or len(coordinates) % 3:
        raise ValueError(f"Expected an ASCII STL with complete triangles: {path}")

    vertices: list[tuple[float, float, float]] = []
    index_by_vertex: dict[tuple[float, float, float], int] = {}
    triangle_vertices: list[int] = []
    for coordinate in coordinates:
        vertex = tuple(float(value) for value in coordinate)
        index = index_by_vertex.get(vertex)
        if index is None:
            index = len(vertices)
            index_by_vertex[vertex] = index
            vertices.append(vertex)
        triangle_vertices.append(index)

    triangles = tuple(
        tuple(triangle_vertices[index : index + 3])
        for index in range(0, len(triangle_vertices), 3)
    )
    return Mesh(tuple(vertices), triangles)


def object_model_xml(part: Part) -> str:
    vertex_xml = "\n".join(
        f'     <vertex x="{x:.6f}" y="{y:.6f}" z="{z:.6f}"/>'
        for x, y, z in part.mesh.vertices
    )
    triangle_xml = "\n".join(
        f'     <triangle v1="{a}" v2="{b}" v3="{c}"/>'
        for a, b, c in part.mesh.triangles
    )
    return f'''<?xml version="1.0" encoding="UTF-8"?>
<model unit="millimeter" xml:lang="en-US" xmlns="http://schemas.microsoft.com/3dmanufacturing/core/2015/02" xmlns:BambuStudio="http://schemas.bambulab.com/package/2021" xmlns:p="http://schemas.microsoft.com/3dmanufacturing/production/2015/06" requiredextensions="p">
 <metadata name="BambuStudio:3mfVersion">1</metadata>
 <resources>
  <object id="{part.inner_id}" p:UUID="{uuid.uuid4()}" type="model">
   <mesh>
    <vertices>
{vertex_xml}
    </vertices>
    <triangles>
{triangle_xml}
    </triangles>
   </mesh>
  </object>
 </resources>
</model>'''


def wrapper_object_xml(part: Part) -> str:
    return f'''  <object id="{part.wrapper_id}" p:UUID="{uuid.uuid4()}" type="model">
   <components>
    <component p:path="/3D/Objects/{part.object_filename}" objectid="{part.inner_id}" p:UUID="{uuid.uuid4()}" transform="1 0 0 0 1 0 0 0 1 0 0 0"/>
   </components>
  </object>
'''


def build_item_xml(part: Part) -> str:
    return (
        f'  <item objectid="{part.wrapper_id}" p:UUID="{uuid.uuid4()}" '
        f'transform="1 0 0 0 1 0 0 0 1 {part.x:.3f} {part.y:.3f} 0" printable="1"/>\n'
    )


def model_settings_object_xml(part: Part) -> str:
    escaped_name = html.escape(part.name, quote=True)
    face_count = len(part.mesh.triangles)
    return f'''  <object id="{part.wrapper_id}">
    <metadata key="name" value="{escaped_name}"/>
    <metadata key="extruder" value="{part.extruder}"/>
    <metadata face_count="{face_count}"/>
    <part id="{part.inner_id}" subtype="normal_part" uuid="{uuid.uuid4()}">
      <metadata key="name" value="{escaped_name}"/>
      <metadata key="matrix" value="1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"/>
      <metadata key="source_file" value="YANA HC-SR04 EYE HOUSING"/>
      <metadata key="source_object_id" value="0"/>
      <metadata key="source_volume_id" value="0"/>
      <metadata key="source_offset_x" value="0"/>
      <metadata key="source_offset_y" value="0"/>
      <metadata key="source_offset_z" value="0"/>
      <mesh_stat face_count="{face_count}" edges_fixed="0" degenerate_facets="0" facets_removed="0" facets_reversed="0" backwards_edges="0"/>
    </part>
  </object>
'''


def plate_xml(parts: tuple[Part, ...]) -> str:
    instances = "".join(
        f'''    <model_instance>
      <metadata key="object_id" value="{part.wrapper_id}"/>
      <metadata key="instance_id" value="0"/>
      <metadata key="identify_id" value="{20600 + index}"/>
    </model_instance>
'''
        for index, part in enumerate(parts)
    )
    return f'''  <plate>
    <metadata key="plater_id" value="16"/>
    <metadata key="plater_name" value="YANA HC-SR04 EYES"/>
    <metadata key="locked" value="false"/>
    <metadata key="filament_map_mode" value="Auto For Flush"/>
    <metadata key="filament_maps" value="1 1 1 1"/>
{instances}  </plate>
'''


def insert_before(text: str, marker: str, addition: str) -> str:
    if marker not in text:
        raise ValueError(f"Required marker not found: {marker}")
    return text.replace(marker, addition + marker, 1)


def merge(source: Path, destination: Path, front_stl: Path, back_stl: Path) -> None:
    parts = (
        Part("YANA HC-SR04 FRONT - YELLOW", read_ascii_stl(front_stl), 88, 89, "object_1000.model", 2, 1012.0, -780.0),
        Part("YANA HC-SR04 BACK - GRAY", read_ascii_stl(back_stl), 90, 91, "object_1001.model", 3, 1084.0, -780.0),
    )

    with zipfile.ZipFile(source, "r") as archive:
        files = {info.filename: archive.read(info.filename) for info in archive.infolist()}

    main_model = files["3D/3dmodel.model"].decode("utf-8")
    main_model = insert_before(
        main_model,
        " </resources>",
        "".join(wrapper_object_xml(part) for part in parts),
    )
    main_model = insert_before(
        main_model,
        " </build>",
        "".join(build_item_xml(part) for part in parts),
    )
    files["3D/3dmodel.model"] = main_model.encode("utf-8")

    relationships = files["3D/_rels/3dmodel.model.rels"].decode("utf-8")
    relationships = insert_before(
        relationships,
        "</Relationships>",
        "".join(
            f' <Relationship Target="/3D/Objects/{part.object_filename}" Id="rel-hcsr04-{index}" Type="http://schemas.microsoft.com/3dmanufacturing/2013/01/3dmodel"/>\n'
            for index, part in enumerate(parts, start=1)
        ),
    )
    files["3D/_rels/3dmodel.model.rels"] = relationships.encode("utf-8")

    settings = files["Metadata/model_settings.config"].decode("utf-8")
    settings = insert_before(
        settings,
        "  <plate>",
        "".join(model_settings_object_xml(part) for part in parts),
    )
    settings = insert_before(settings, "  <assemble>", plate_xml(parts))
    files["Metadata/model_settings.config"] = settings.encode("utf-8")

    for part in parts:
        files[f"3D/Objects/{part.object_filename}"] = object_model_xml(part).encode("utf-8")

    destination.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(destination, "w", compression=zipfile.ZIP_DEFLATED) as archive:
        for filename, data in files.items():
            archive.writestr(filename, data)

    print(f"Wrote {destination} with plate 16: {parts[0].name}, {parts[1].name}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("destination", type=Path)
    parser.add_argument("front_stl", type=Path)
    parser.add_argument("back_stl", type=Path)
    args = parser.parse_args()
    merge(args.source, args.destination, args.front_stl, args.back_stl)


if __name__ == "__main__":
    main()
