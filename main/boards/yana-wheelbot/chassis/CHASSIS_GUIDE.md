# Yana Wheelbot Chassis — Print & Assembly Guide

(English | [Tiếng Việt](CHASSIS_GUIDE_vi.md) | [한국어](CHASSIS_GUIDE_ko.md))

This document covers the chassis:

- [`yana-wheelbot-chassis-final.stl`](yana-wheelbot-chassis-final.stl): the
  file to load into your slicer.
- [`yana_wheelbot_chassis_v2.scad`](yana_wheelbot_chassis_v2.scad): the
  OpenSCAD source for adjusting dimensions.
- Current exported STL size: **210 x 142 x 43.85 mm** (the extra width past
  the 122mm deck comes from the arm horn paddle described below).

The chassis is a functional integration deck with mounts for the ESP32-S3,
display, audio, sensors, four servos, battery, and two power boards. It is
not yet a cosmetic Wall-E-style outer shell.

> **Important:** cloned PCB, battery, speaker, switch, and servo-horn
> dimensions can differ from the listing photos. Before the final print,
> measure the real components with calipers and adjust the variables at the
> top of the `.scad` file if the mismatch is more than about 0.5 mm.

> **New — arm/neck horn paddles, hole spacing NOT yet verified.** The STL
> now includes two small separate pieces — a paddle for the arm servo and a
> bracket for the neck servo — that screw onto each servo's own stock horn
> (2 holes, 8mm apart, sized for an M2 self-tap screw). That 8mm spacing is
> a typical SG90-class figure, **not measured from the actual horn you'll
> use**. Different horn brands vary by a few mm. Hold the real horn up to
> the printed paddle before committing glue or a permanent screw — if the
> holes are off, adjust `horn_hole_spacing` in the `.scad` file and
> re-export before printing again.

## 1. Choosing a print material

### Recommended: PETG for real-world use

PETG handles impact, vibration, and heat inside the robot body better than
PLA. It is the most balanced choice for a chassis carrying servos.

Starting profile for a 0.4 mm nozzle:

| Setting | Starting value |
|---|---:|
| Layer height | 0.20 mm |
| Wall count | 4 perimeters/walls |
| Top/bottom | 5 layers or more |
| Infill | 25–35% gyroid or cubic |
| Nozzle temp | 235–250 °C, per filament |
| Bed temp | 75–85 °C, per filament |
| Fan | 30–50% |
| Brim | 5–8 mm if corners lift |

### PLA/PLA+

Good for test-fit prints since it's easy to print and warps less. Do not
leave a PLA robot inside a car, near a sun-facing window, or near a heat
source. Starting profile: 205–220 °C nozzle, 55–65 °C bed, 4 walls, 25–30%
infill.

### ASA/ABS

Only use this if your printer has an enclosed chamber and you're already
comfortable managing shrinkage. Suited for a robot that will regularly sit
in a hot environment. Re-check pocket dimensions after printing, since
ASA/ABS shrinks more than PETG/PLA.

### TPU

Do not use TPU for the main deck or servo mounts. TPU is only suitable for
printing tires, anti-slip pads, vibration feet, and cable inserts.

### Not recommended

- Brittle resin for load-bearing mounts and servo attachment areas.
- Silk PLA for structural parts.
- Too few wall shells; servos create repeated vibration load at their mount
  points.

## 2. Orientation and supports

1. Place the deck's large flat face down against the print bed.
2. Do not print the chassis standing on its side; this weakens the servo
   mounts and wastes support material.
3. Enable support **from build plate only** for the TFT bezel area if your
   slicer flags a long bridge.
4. Check the layer preview closely, especially at the TFT bezel, the
   battery-wire slot, and the servo mounts.
5. Use a brim if PETG/ABS lifts at the four corners.
6. After printing, remove supports and test-fit each component before
   soldering any wires.

The four main mounting holes are about 3.4 mm in diameter, sized for an
M3 through-bolt. The PCB pockets currently use retaining walls and support
rails, not per-board mounting-hole patterns.

## 3. Core hardware BOM

The links below are a purchasing list assembled independently for the Yana
project — not carried over from KST's own sales links. Listings can change
seller, price, or variant; verify the exact spec before ordering.

| # | Component | Qty | Spec to buy | Source | GPIO/bus | Reference CAD envelope | Link |
|---:|---|---:|---|---|---|---|---|
| 1 | ESP32-S3 N16R8 | 1 | WROOM-1, 16 MB flash, 8 MB PSRAM, 44-pin DevKit | 5 V/VIN; 3V3 logic | — | board approx. 63–70 x 25–28 mm | [Coupang](https://www.coupang.com/vp/products/9476820138) |
| 2 | ST7735 TFT | 1 | 1.8 inch, 128x160, SPI | 3V3 | MOSI 11, SCK 12, DC 10, CS 13, RST 14 | approx. 56 x 34 mm | [Coupang](https://www.coupang.com/vp/products/7991533839) |
| 3 | INMP441 | 1 | Digital MEMS I2S mic | 3V3 | WS 4, SCK 5, SD 6 | approx. 15–20 x 10–15 mm | [Coupang](https://www.coupang.com/vp/products/8792464304?itemId=25592833527&vendorItemId=94254523602) |
| 4 | MAX98357A | 1 | I2S Class-D mono, approx. 3 W | 5 V rail | LRC 15, BCLK 16, DIN 17 | approx. 19 x 18 mm | [Coupang](https://www.coupang.com/vp/products/9649666019) |
| 5 | Speaker | 1 | 3 W, 4 ohm | MAX98357A | — | measure the real speaker; CAD currently reserves approx. Ø42 mm | [Coupang](https://www.coupang.com/vp/products/9687303942?itemId=28970688668&vendorItemId=95932788332) |
| 6 | VL6180X | 1 | ToF, I2C | 3V3 | SDA 1, SCL 2 | PCB approx. 18–25 mm | [Coupang](https://www.coupang.com/vp/products/7206616775?itemId=18226765538&vendorItemId=85374303210) |
| 7 | TTP223 | 1 | Capacitive touch | 3V3 | OUT 7 | approx. 14 x 11 mm | [Coupang](https://www.coupang.com/vp/products/9337846580?itemId=27690418458&vendorItemId=94652272760) |
| 8 | 360° servo | 2 | Micro **continuous-rotation** servo | 5 V servo rail | left 47, right 45 | SG90-class, approx. 23 x 12 x 29 mm | Not yet finalized; do not accidentally buy the 180° version |
| 9 | 180° servo | 2 | Micro positional servo for arm and neck | 5 V servo rail | arm 20, neck 21 | approx. 23 x 12 x 29 mm | [Coupang](https://www.coupang.com/vp/products/190778115?itemId=9577817213&vendorItemId=76862407783) |
| 10 | Discrete LED | 2 | White or warm-white, with current-limiting resistor | GPIO via resistor | left 3, right 18 | measure the actual LED | [Coupang](https://www.coupang.com/vp/products/8202481908?itemId=23517797018&vendorItemId=90544172857) |
| 11 | LiPo | 1 | 1S 3.7 V, approx. 2000 mAh, adequate discharge current | — | — | measure the actual cell | [Coupang](https://www.coupang.com/vp/products/9430136637?itemId=28036955286&vendorItemId=92625474566) |
| 12 | TP4056 Type-C | 1 | Charger **with protection** for a 1S cell | USB 5 V | — | approx. 25–30 x 17–20 mm | Not yet finalized |
| 13 | Boost converter | 1 | 3.x V to regulated 5 V, current-rated for four servos | LiPo via switch | — | per actual board; CAD pocket 42 x 24 mm | [Coupang](https://www.coupang.com/vp/products/9692004769?itemId=28987690226&vendorItemId=95916593087) |
| 14 | Mini ON/OFF switch | 1 | Latching slide switch, rated for the boost input current | power line | — | per actual switch | [Coupang](https://www.coupang.com/vp/products/8758411816?itemId=25465322348&vendorItemId=95044703259) |

An L298N is not needed for this configuration. The firmware can keep the
L298N backend available, but the hardware here uses two continuous-rotation
servos instead.

## 4. Supporting materials

| Material | Suggested use |
|---|---|
| 20–22 AWG silicone wire | power line, boost, and servo power bus |
| 26–28 AWG wire | GPIO, I2C, I2S signal lines |
| Locking JST connectors | battery, power, speaker, and any removable assembly |
| 2.54 mm headers | module prototyping |
| Perfboard or power-distribution board | clean 5 V and GND fan-out |
| Low-ESR bulk capacitors | placed near the servo bus; start at 1000–2200 µF, 6.3 V or higher |
| LED resistors | sized per LED; a good starting test value is 330 ohm |
| Heat-shrink | insulating solder joints |
| M3 screws | the four Ø3.4 mm chassis mounting holes |
| Servo screws and servo horns | wheels, arm, and neck |
| Hook-and-loop strap | securing the battery through the two built-in slots |
| Small cable ties | securing the switch, wires, and light modules |
| Thin foam tape | vibration damping; do not place on the antenna or any hot component |
| USB-C data cable | flashing and debugging the ESP32-S3 |

Do not use Dupont jumpers as a permanent connection on a moving robot.
Dupont wires are fine for bench testing only; sustained vibration can work
the pins loose over time.

## 5. Required power architecture

```text
USB-C 5 V
    │
    ▼
TP4056 charger + protection
    │ B+/B- or OUT+/OUT- per the board's actual labeling
    ▼
1S LiPo 3.0–4.2 V
    │
    ▼
ON/OFF switch
    │
    ▼
5 V regulated boost
    ├──────── ESP32-S3 5V/VIN pin
    ├──────── MAX98357A VIN
    └──────── all four servos VCC

ESP32-S3 3V3 pin
    ├──────── ST7735
    ├──────── INMP441
    ├──────── VL6180X
    └──────── TTP223

All GND is common.
```

Safety rules:

- The TP4056 is a charging circuit, **not a boost converter**.
- Do not power the servos from the ESP32's 3V3 pin.
- Do not connect the 3.7 V battery directly to the 5 V bus.
- Do not feed 5 V into a 3V3 pin.
- The boost converter must handle both the continuous current and the peak
  current when servos start or stall.
- A 2000 mAh rating only states capacity, not guaranteed discharge
  capability.
- For the first builds, power the robot off while charging. A typical
  TP4056 has no proper power-path/load-sharing to run a heavy load and
  charge at the same time.
- Measure the boost output before plugging in the ESP32. Only connect it
  once the voltage is stable near 5 V.

## 6. Wiring tables

### ST7735

| ST7735 | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| MOSI/SDA | GPIO11 |
| SCK/CLK | GPIO12 |
| DC/A0 | GPIO10 |
| CS | GPIO13 |
| RST/RES | GPIO14 |
| BL/LED | see note below |

The Yana Wheelbot firmware currently uses **GPIO9** to control backlight
brightness. If your backlight module draws more current than the GPIO can
safely supply, drive it through a transistor/MOSFET instead of pulling the
backlight LED directly from the GPIO. KST's simpler reference wiring ties
BL straight to 3V3, but that loses firmware brightness control.

### INMP441

| INMP441 | ESP32-S3 |
|---|---|
| VDD | 3V3 |
| GND | GND |
| L/R | GND |
| WS | GPIO4 |
| SCK/BCLK | GPIO5 |
| SD | GPIO6 |

Point the mic hole outward and don't let glue block the sound port.

### MAX98357A and speaker

| MAX98357A | Connection |
|---|---|
| VIN | 5 V bus from the boost converter |
| GND | common GND |
| LRC/LRCLK | GPIO15 |
| BCLK | GPIO16 |
| DIN | GPIO17 |
| GAIN | GND, per the build sheet configuration |
| SD | VIN, per the build sheet configuration |
| SPK+ / SPK- | both speaker terminals — never tie one speaker terminal to GND |

### VL6180X

| VL6180X | ESP32-S3 |
|---|---|
| VIN/VCC | 3V3 |
| GND | GND |
| SDA | GPIO1 |
| SCL | GPIO2 |

### TTP223

| TTP223 | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| OUT | GPIO7 |

### Servos

| Servo | Signal | Power |
|---|---:|---|
| Left wheel, 360° | GPIO47 | 5 V servo rail + common GND |
| Right wheel, 360° | GPIO45 | 5 V servo rail + common GND |
| Arm, 180° | GPIO20 | 5 V servo rail + common GND |
| Neck, 180° | GPIO21 | 5 V servo rail + common GND |

Brown/black is usually GND, red is usually VCC, and orange/yellow/white is
usually signal on servo wires — but always confirm against the datasheet or
label of the exact servo you bought.

### LEDs

| LED | GPIO |
|---|---:|
| Left | GPIO3 |
| Right | GPIO18 |

Each LED needs its own current-limiting resistor. Never wire an LED
straight from GPIO to GND without one. High-power or high-current LEDs
need a transistor driver.

### Reserved pins

Do not use GPIO36 or GPIO37 on this configuration's ESP32-S3 N16R8 — they
are tied to the module's PSRAM.

## 7. Component layout on the chassis

OpenSCAD convention: `+X` is front, `+Y` is left.

```text
                     FRONT (+X)
              [ST7735 TFT bezel]

 [ToF, facing down] [wheel servo] [ESP32-S3] [wheel servo]
 [mic] [touch] [amp] [boost] [TP4056]
 [neck servo] [arm servo] [battery, low] [speaker + grille]

                     REAR (-X)
```

- The battery sits low, near the center, held by a strap.
- The ESP32's USB-C end stays accessible; don't route the antenna against
  the battery, the speaker magnet, or the boost converter.
- The speaker sits directly on the grille, with the diaphragm venting
  outward.
- The mic stays away from the speaker and amplifier to reduce
  feedback/echo.
- The ToF sensor looks straight down through an opening, positioned ahead
  of the wheel axis for early edge detection.
- The TTP223 can sit behind a thin plastic layer; the PCB itself doesn't
  need to be exposed.
- The switch mounts into the two general-purpose slots using a cable tie
  or a small adapter plate.
- LEDs don't have a fixed socket yet, since their exact package size
  hasn't been measured.

## 8. Mechanical assembly order

1. Print the chassis and clean up supports.
2. Dry-fit every component, without soldering or gluing anything yet.
3. Mount the two wheel servos; the shafts must point outward on both sides
   at the same height.
4. Attach the horn/wheel and spin it by hand to confirm nothing rubs the
   deck or the wiring.
5. Mount the arm servo and neck servo; check the full 0–180° sweep on both.
6. Insert the TFT from the bezel's open side, without forcing it against
   the glass or the display cable.
7. Seat the speaker on its ring, facing the grille; don't puncture the
   diaphragm.
8. Seat the ToF sensor and sight through from below to confirm the optical
   window isn't blocked by plastic.
9. Seat the ESP32, keeping the USB-C end and antenna area clear.
10. Seat the TP4056, boost converter, MAX98357A, TTP223, and INMP441 into
    their pockets.
11. Thread the strap through the two slots and secure the battery; don't
    over-tighten and deform the cell.
12. Route power wiring first, signal wiring second; keep the mic wire away
    from the speaker and servo wiring.
13. Add strain relief at the battery wire, the speaker wire, and each servo
    lead.
14. Only close up the outer shell after a full-load test has passed.

## 9. Power-up and test sequence

### Stage A: power

1. Confirm battery polarity.
2. Connect the battery to the TP4056's B+/B- pins per the board's actual
   labeling.
3. Connect the protected output through the switch to the boost converter
   input.
4. Do not plug in the ESP32 or any servo yet.
5. Turn the switch on and measure the boost output.
6. Trim the boost to roughly 5 V if the board has a trim potentiometer.
7. Power off and check for any 5 V/GND short.

### Stage B: ESP32 and logic

1. Feed 5 V into the ESP32's 5V/VIN pin.
2. Confirm the ESP32 boots reliably.
3. Measure the 3V3 rail.
4. Connect the ST7735 and test the display.
5. Connect the INMP441 and test the mic.
6. Connect the VL6180X and check the distance reading.
7. Connect the TTP223 and test touch.

### Stage C: audio and servos

1. Connect the MAX98357A, then connect the speaker only after that.
2. Test at low volume first.
3. Test each servo individually, without heavy load attached.
4. Determine the stop point of both 360° servos.
5. Test the left wheel, right wheel, arm, and neck in sequence.
6. Only then run all four servos together with Wi-Fi, the display, and
   audio active.

If the ESP32 resets while a servo is running, check the boost converter,
battery, wire gauge, connectors, and bulk capacitor first before assuming
it's a firmware bug.

## 10. Building and flashing the Yana firmware

Use ESP-IDF 6.0.2 where possible:

```sh
source /path/to/esp-idf/export.sh
idf.py --version
python3 scripts/build.py --list-boards
python3 scripts/build.py yana-wheelbot --name yana-wheelbot
```

This document's hardware uses the ST7735 and VL6180X. Enable both of the
following under **Yana Wheelbot** in `idf.py menuconfig`, then rebuild:

```text
CONFIG_YANA_WHEELBOT_DISPLAY_ST7735=y
CONFIG_YANA_WHEELBOT_TOF_VL6180X=y
```

After changing the configuration:

```sh
idf.py fullclean
idf.py build
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

Replace `/dev/cu.usbmodemXXXX` with the actual serial port. Use a USB-C
cable that carries data. KST's firmware is reference-only; the Yana build
should always come from this repository's own source.

## 11. Distinguishing KST reference material from Yana's firmware

KST's public materials are used strictly as **hardware reference**:
component names, GPIOs, the 5 V power requirement, common GND, and the
GPIO36/GPIO37 warning. None of their firmware, guide text, or marketing
links are copied into Yana.

| KST reference item | Actual Yana behavior |
|---|---|
| Web flasher, merged binary, and `0x0` offset | The Yana firmware is built from source with ESP-IDF and flashed with `idf.py`; KST's offset does not apply to Yana builds |
| `KST-Robot-Ai-xxxx` AP and `/robot` dashboard | Not treated as a Yana commitment; use the Yana firmware's own provisioning flow and the client in `apps/controller-web` |
| Holding TTP223 for ~5 seconds at power-on to change Wi-Fi | **Not implemented in Yana Wheelbot yet**; TTP223 currently only handles a double-tap to toggle chat |
| Activation at `xiaozhi.me` and the "Hi Lily" wake word | Depends on backend/configuration; not a fixed property of the Yana chassis or board |
| ToF disabled by default | Yana currently defaults the cliff sensor to **enabled**, 50 mm threshold, saved to NVS |

Yana's local control uses a WebSocket at `ws://<IP>:8080/ws?token=<token>`
and requires a pairing token. See
[controller-web](../../../../apps/controller-web/README.md) and the parent
Yana Wheelbot board documentation.

## 12. Checklist before running on the floor

- [ ] Battery isn't swollen, isn't punctured, and polarity is correct.
- [ ] The TP4056 is the correct 1S variant with protection.
- [ ] The boost converter stays stable near 5 V under load.
- [ ] No 5 V/GND short.
- [ ] All GND is common.
- [ ] The ESP32 boots and doesn't brown out.
- [ ] The USB-C port is still reachable.
- [ ] The ST7735 displays with the correct orientation.
- [ ] The mic picks up sound clearly, with no glue over the port.
- [ ] The speaker isn't buzzing, and the diaphragm doesn't touch the
      chassis.
- [ ] Both wheel servos stop cleanly on a stop command.
- [ ] Left/right direction is correct; apply reverse calibration if needed.
- [ ] The arm and neck don't strike the chassis at either end of their
      travel.
- [ ] The ToF sensor faces straight down and isn't blocked by plastic.
- [ ] The cliff sensor is enabled and tested by holding the robot up by
      hand.
- [ ] No wire touches the wheels, tracks, or horns.
- [ ] The battery, power boards, and ESP32 can be removed for maintenance.
- [ ] Full load doesn't reset the ESP32.

## 13. Common failures

| Symptom | Check first |
|---|---|
| ESP32 resets while a servo runs | boost undersized, weak battery discharge, thin power wire, loose connector, missing bulk capacitor |
| Servo buzzes while idle | GND not common, noisy power, signal wire routed next to a power wire, 360° servo stop point off |
| Both wheels drive in opposite directions | use `self.wheelbot.set_servo_reverse`, don't swap the servo's power leads |
| TFT is blank white/black | wrong ST7735/ST7789 build option, wrong CS/DC/RST wiring, BL not powered |
| No sound | wrong BCLK/LRC/DIN wiring, MAX98357A not getting 5 V, SPK+/SPK- wired backward |
| Mic noise/feedback | mic too close to the speaker, mic wire routed next to a servo, speaker volume too high |
| ToF always reports an error | SDA/SCL swapped, no common GND, optical window blocked by the chassis, VL6180X build option not enabled |
| Weak Wi-Fi | ESP32 antenna sits against the battery, speaker magnet, boost converter, or metal object |
| Chassis corners warp | increase brim, clean the bed, reduce draft, adjust bed temperature |
| Component doesn't fit its pocket | re-measure the real board and correct the envelope in the `.scad` file |

## 14. Maintenance

- Periodically check servo screws, horns, and the battery strap.
- Don't pull on the wire to disconnect a JST connector; hold the connector
  housing itself.
- Disconnect the battery before working on power wiring.
- Never charge a cell that's swollen, punctured, or unusually hot.
- After any impact, check for cracks around the servo mounts and the
  chassis mounting holes.
- Keep the OpenSCAD file adjusted to your actual component dimensions on
  hand, so you can reprint the exact version of the robot you're running.
