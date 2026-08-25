# Yana HC-SR04 eye housing

This folder contains a replacement eye housing sized for the HC-SR04 module
linked for the Yana Wheelbot.

## Finished files

- `yana-hc-sr04-eye-front.stl`: yellow front housing.
- `yana-hc-sr04-eye-back.stl`: gray rear cover and neck mounting tab.
- `yana-hc-sr04-eye-housing.3mf`: assembled reference model.
- `fire-in-du-an-ket-hop-hc-sr04.3mf`: the complete Bambu project with the
  two new parts on plate 16, named `YANA HC-SR04 EYES`.
- `hc_sr04_eye_housing.scad`: parametric OpenSCAD source.

## Designed clearances

| Feature | CAD size |
| --- | ---: |
| HC-SR04 PCB pocket | 45.6 x 20.6 mm |
| Transducer openings | 16.7 mm diameter |
| Transducer center spacing | 26.0 mm |
| Front housing | 57 x 30 x 18 mm |
| Rear cover, including neck tab | 57 x 37.1 x 4.6 mm |
| Wire exit | 8 x 5 mm |

The PCB pocket has 0.3 mm clearance per side. Clone modules can vary, so print
the front housing first and test-fit the purchased sensor before printing the
rest of the robot.

## Printing

- Front housing: yellow PLA or PETG, front face on the build plate.
- Rear cover: gray PLA or PETG, outside face on the build plate.
- Nozzle: 0.4 mm.
- Layer height: 0.16 or 0.20 mm.
- Walls: 3.
- Infill: 15-20%.
- Supports: normally not required in the supplied orientation.
- Fasteners: 2 x M2 self-tapping screws for the side bosses.

## Assembly

1. Insert both ultrasonic cans through the two front openings from the rear.
2. Seat the PCB in the 45.6 x 20.6 mm pocket without forcing it.
3. Route VCC, TRIG, ECHO, and GND through the bottom notch.
4. Fit the gray rear cover and secure it with two M2 screws.
5. Attach the neck servo horn through the lower three-hole mounting pattern.
6. Confirm the head can sweep without pulling the four sensor wires.

The front is assigned to filament 2 (yellow) and the back to filament 3 (gray)
in plate 16 of the combined project.
