# Yana Wheelbot

(English | [Tiếng Việt](README_vi.md) | [한국어](README_ko.md))

A wheeled-robot (differential-drive) board on the Yana Robot / XiaoZhi
platform. Adds: a selectable motor backend (continuous-rotation servo or
L298N), an anti-fall ToF sensor, dual LEDs, arm + neck servos, and
switchable display orientation/theme — on top of the platform's existing
voice AI and animated-face display.

## Required parts

| Part | Suggested component | Notes |
|---|---|---|
| MCU | ESP32-S3-WROOM-1 N16R8 (16MB flash / 8MB PSRAM) | `target: esp32s3` in `config.json` |
| Motor driver (pick one) | 2x continuous-rotation (360°) servo | Drives the wheels directly, no separate driver needed |
| | *or* L298N + 2x DC motor | See the IN1-4 pin-convention caveat in "Not yet verified on real hardware" |
| Anti-fall sensor (ToF, I2C) | VL53L0X (default, ~2m range) | |
| | *or* VL6180X / TOF050C (build option, ~200mm range) | See "Build-time variant selection" |
| LEDs | 2x single LED (left/right) | |
| Arm + neck servos | 2x standard angle servo (0-180°) | |
| Touch sensor | TTP223 capacitive touch module | Double-tap toggles chat, same as the boot button |
| Mic | I2S, e.g. INMP441 or equivalent | |
| Speaker | I2S amp, e.g. MAX98357A + 3W/4Ω speaker | Needs its own strong 5V rail — see power note below |
| Display | SPI ST7789 128x160 (default) | |
| | *or* ST7735 (build option) | See "Build-time variant selection" |
| Boot button | already on the ESP32-S3 dev board (GPIO0) | Nothing extra needed |
| Power | Li-ion/LiPo 3.7V, ~2000mAh | |
| | TP4056 Type-C charge + 5V boost module | |
| | ON/OFF power switch | |

**Power note:** the speaker amp and both wheel servos need their own strong
5V rail (not powered straight off the MCU's 3.3V regulator) — a boost
module like TP4056 handles this from a single 3.7V LiPo cell. All grounds
(MCU, amp, servos, sensors) must still be tied together.

## Default wiring

![Yana Wheelbot wiring diagram](wiring-diagram.svg)

Default GPIO pins are taken from the [KST AI Robot](https://ai.kenhsangtao.com/)'s
publicly published wiring diagram (see the "Credit" section below) — every
pin below is confirmed against that diagram except the display backlight,
which is this project's own free-pin pick (flagged in the Notes column).
All definitions live in `config.h`.

| Part | Signal | GPIO | Notes |
|---|---|---|---|
| Motor (L298N) | IN1 (left, PWM/EN) | 38 | Remappable at runtime via `self.wheelbot.set_motor_pins` |
| | IN2 (left, DIR) | 39 | |
| | IN3 (right, PWM/EN) | 40 | |
| | IN4 (right, DIR) | 41 | |
| Wheel servo (if using servo instead of L298N) | Left | 47 | |
| | Right | 45 | |
| ToF sensor (I2C) | SDA | 1 | |
| | SCL | 2 | |
| LED | Left | 3 | |
| | Right | 18 | |
| Arm servo | Signal | 20 | |
| Neck servo | Signal | 21 | |
| Touch sensor (TTP223) | OUT | 7 | Active-high; double-tap toggles chat |
| Mic (I2S) | WS | 4 | |
| | SCK | 5 | |
| | DIN | 6 | |
| Speaker (I2S) | DIN | 17 | |
| | BCLK | 16 | |
| | LRCK | 15 | |
| Display (SPI) | Backlight | 9 | This project's own free-pin pick — KST's module ties BL straight to 3V3, no software control |
| | MOSI | 11 | |
| | CLK | 12 | |
| | DC | 10 | |
| | RST | 14 | |
| | CS | 13 | |

Motor IN1-4 pins (and the servo/L298N backend choice) can be changed at
runtime via an MCP tool, persisted to NVS, no reflash required — the values
above are only the first-boot defaults.

**Do not use GPIO36/GPIO37** — reserved for PSRAM on the ESP32-S3-WROOM-1
N16R8 module.

## Build-time variant selection

Defaults to ST7789 + VL53L0X. If your actual parts are ST7735 and/or
VL6180X, enable these two Kconfig options before building (`idf.py
menuconfig` → "Yana Wheelbot", or edit `sdkconfig` directly):

```
CONFIG_YANA_WHEELBOT_DISPLAY_ST7735=y   # use the ST7735 driver instead of ST7789
CONFIG_YANA_WHEELBOT_TOF_VL6180X=y      # use the VL6180X sensor instead of VL53L0X
```

Run `idf.py fullclean` and rebuild after changing either option.

## Control protocol

This board speaks 2 protocols, sharing the same set of MCP tools:

**1. Local control (LAN, no cloud round-trip)** — works the same way as
`main/boards/otto-robot`: the board runs a WebSocket server on port 8080
that accepts messages in the same JSON-RPC 2.0 envelope MCP uses, with no
backend AI in the loop. `apps/controller-web` (in this repo) is a ready-made
web client for this protocol.

**Requires a pairing token** — a random 6-digit code generated once on
first boot and persisted in NVS, required as a `?token=<code>` query param
on the handshake URL. Without it, anyone on the same Wi-Fi network could
otherwise drive the robot or remap its motor GPIOs with zero authentication.
Retrieve it via the `self.local_control.get_token` MCP tool over an
already-authenticated channel (e.g. ask through voice/cloud control), or
rotate it with `self.local_control.rotate_token`.

**Connect:** `ws://<device-ip>:8080/ws?token=<code>`

```json
{"jsonrpc":"2.0","method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{}},"id":1}
{"jsonrpc":"2.0","method":"tools/list","params":{},"id":2}
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.wheelbot.move_forward","arguments":{"duration_ms":2000,"speed":80}},"id":3}
```

**2. Voice control (cloud/backend AI)** — the board acts as the MCP
**server** (owns and executes the tools), the backend AI acts as the MCP
**client** — the standard XiaoZhi/yana-robot WebSocket protocol
(`docs/websocket.md`, `docs/mcp-protocol.md` at the repo root). No firmware
change is needed to switch backends — just point the board's server URL at
any backend speaking this same protocol (for example, the `Yana-AI`
project's `tools/yana-web/robot.js`).

### Tool list

| Tool | Arguments | Notes |
|---|---|---|
| `self.wheelbot.move_forward` | `duration_ms` (0-30000, default 2000), `speed` (0-100, default 80) | a new move preempts one already in flight (breaks out of it immediately), not just a queued one |
| `self.wheelbot.move_backward` | same | |
| `self.wheelbot.turn_left` | same | in-place turn |
| `self.wheelbot.turn_right` | same | in-place turn |
| `self.wheelbot.stop` | — | immediate e-stop, interrupts any in-flight move |
| `self.wheelbot.set_motor_type` | `type`: `"servo"` \| `"l298n"` | persists, reboots to apply; any other string is rejected with an error, not silently defaulted |
| `self.wheelbot.set_motor_pins` | `in1`,`in2`,`in3`,`in4` (GPIO numbers) | L298N pins only; persists, reboots to apply; rejects duplicate pins and reserved GPIOs (flash/PSRAM, nonexistent, or strapping pins) before saving |
| `self.wheelbot.get_motor_config` | — | returns current backend + pins + calibration as JSON |
| `self.wheelbot.get_control_status` | — | is a move currently running, and which transport (`local_ws` or `cloud_voice`) sent the current/last move — see "Command source" below |
| `self.wheelbot.set_servo_stop_pulse` | `microseconds` (1000-2000, default 1500) | live-apply; servo backend only |
| `self.wheelbot.set_servo_reverse` | `side`: `"left"`\|`"right"`, `reversed` (bool) | live-apply; servo backend only; any other `side` value is rejected with an error |
| `self.cliff_sensor.set_enabled` | `enabled` (bool) | |
| `self.cliff_sensor.set_threshold` | `threshold_mm` (5-500, default 50) | stops if the downward sensor reads at/above this (or errors) — sensor assumed downward-facing |
| `self.cliff_sensor.get_config` | — | |
| `self.cliff_sensor.test_now` | — | returns one mm reading |
| `self.led.set_mode` | `mode`: `follow_state`\|`both_on`\|`both_off`\|`left_only`\|`right_only` | `follow_state` mirrors the device-state color/blink behavior every other board's LED already has |
| `self.led.get_mode` | — | |
| `self.arm.set_angle` / `self.neck.set_angle` | `angle` (0-180) | |
| `self.arm.wave` | — | canned gesture, non-blocking |
| `self.neck.turn` | `direction`: `left`\|`right`\|`center` | |
| `self.arm.release` / `self.neck.release` | — | stops PWM so the servo goes limp |
| `self.screen.set_theme` | `theme`: `light`\|`dark`\|`ocean` | already generic in `mcp_server.cc`; `ocean` is this board's addition |
| `self.screen.set_orientation` | `orientation`: `portrait`\|`landscape` | persists, reboots to apply |
| `self.local_control.get_token` | — | returns the current local-WS pairing token |
| `self.local_control.rotate_token` | — | generates and returns a new pairing token, invalidating the old one |

### Command source

Move commands can arrive from either transport at any time (see "Control
protocol" above) with no arbitration between them beyond "the newest command
wins, immediately" — there is no lease/lockout concept, so a phone app and a
voice session can both drive the robot in the same window, and whichever
command lands last takes over. `self.wheelbot.get_control_status` reports
which transport (`local_ws` or `cloud_voice`) sent the current or most
recent move, so a UI can at least show when the *other* channel just took
over, even though it can't reserve control. `command_source.h` tags this
per synchronous MCP call; it does not touch shared platform code
(`mcp_server.cc`, `application.cc`).

## Credit

The GPIO defaults in `config.h` (motor, ToF, LED, arm/neck, touch, mic,
speaker, display pins) are aligned with the
[KST AI Robot](https://ai.kenhsangtao.com/)'s publicly published wiring
diagram and firmware center (`kenhsangtao.github.io/robotai`) — a real,
community-built ESP32-S3 robot from the Vietnamese "Kênh Sáng Tạo" channel
with a nearly identical feature set (dual motor backend, arm/neck servos,
ToF anti-fall, dual LED, touch sensor, voice AI). Only pin *numbers* and
publicly listed part names were used; no code, artwork, or text was copied
from their firmware (a closed binary with no stated reuse license) or their
website. `wiring-diagram.svg` above is this project's own original diagram,
redrawn from those public facts — not a copy of their diagram image. This
board's firmware, board definition, and MCP tools are written
independently.

## Not yet verified on real hardware

Everything below is only build-verified (compiles cleanly) — **no real
robot has run this yet** — verify before fully trusting it:

- **L298N pin convention**: this board PWMs `in1`/`in3` directly for speed
  and uses `in2`/`in4` as static direction pins — it does **not** drive the
  standard module's separate ENA/ENB enable pins at all. This is a real,
  documented technique (see e.g. the Raspberry Pi forums thread ["Using ENA
  or IN1/2 PWM for L298N motor speed control"](https://forums.raspberrypi.com/viewtopic.php?t=90243)),
  but it only works if your L298N module's ENA/ENB jumpers are **left
  installed** (tying them permanently high) — the factory-default state on
  most breakout boards. If your module has no such jumpers, or they've been
  removed, ENA/ENB must be wired to a permanent 5V/3.3V source (or this
  driver extended to also control them) before this board's motors will
  spin at all. Separately, the KST AI Robot's firmware strings suggest
  their own "mini motor driver" module uses static (non-PWM) levels
  directly on IN1-4 instead — a different module design than a standard
  L298N breakout; that detail came from binary strings, not documented
  source, so treat it as a signal to check your specific module against,
  not a confirmed spec. Verify against your actual module before trusting
  `l298n_motor_driver.cc`.
- **Continuous-rotation servo speed mapping** (`servo_motor_driver.cc`):
  1500µs stop / 1000-2000µs full range matches the standard, widely-
  documented continuous-rotation servo convention (e.g.
  [dronebotworkshop.com/servoguide](https://dronebotworkshop.com/servoguide/),
  [SparkFun's continuous-rotation servo guide](https://learn.sparkfun.com/tutorials/continuous-rotation-servo-trigger-hookup-guide/continuous-rotation-servo-motors)),
  not a guess. What's genuinely unverified is the exact stop point of *your*
  specific physical servo — every unit varies slightly, which is exactly
  why this is exposed as a runtime tool (`self.wheelbot.set_servo_stop_pulse`)
  rather than a fixed constant; "nulling" a real servo by hand is the
  standard calibration step here, not a bug to fix in code.
- **VL53L0X driver** (`vl53l0x.cc`): implements presence detection + the
  documented single-shot ranging trigger/poll/read sequence only. It does
  **not** implement ST's full reference calibration (SPAD, timing budget,
  signal-rate-limit tuning). Verify readings against a known distance before
  trusting them for anti-fall safety.
- **VL6180X driver** (`vl6180x.cc`): implements the mandatory ~30-register
  SR03 private init sequence from AN4545 (cross-checked against
  pololu/vl6180x-arduino and Adafruit_VL6180X, both MIT — register values
  only, no code copied), gated on the `SYSTEM_FRESH_OUT_OF_RESET` flag as
  the datasheet requires. It does **not** implement AN4545's separate
  "recommended" tuning (readout averaging, ALS gain, interrupt config, VHV
  repeat rate) — those affect measurement quality, not whether ranging
  works at all. Still unverified on real hardware.
- **Display orientation**: applied via a reboot rather than a live re-layout,
  since re-invoking `esp_lcd_panel_swap_xy`/`mirror` at runtime without a
  reboot is not verified safe on this panel/driver combination.
- **TTP223 touch sensor**: wired as a plain active-high digital `Button`
  (`OnDoubleClick`) — TTP223 modules are cheap and sometimes noisy/self-
  triggering without a stable 3V3 rail; debounce behavior has not been
  tested on real hardware.
