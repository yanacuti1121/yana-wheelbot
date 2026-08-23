# Yana Robot

(English | [Tiếng Việt](README_vi.md) | [한국어](README_ko.md))

Independent ESP32-S3 voice-AI robotics platform derived from
[XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32) (MIT) — motor/servo
control, anti-fall safety, and MCP-based voice + web control.

<img src="docs/mcp-based-graph.jpg" alt="Control everything via MCP" width="320">

## What this actually is, right now

This started as a clone of XiaoZhi AI Chatbot, a genuinely open-source
(MIT) ESP32 voice-assistant firmware. The inherited platform — WebSocket
and MQTT+UDP transports, offline wake-word detection, streaming ASR/LLM/TTS,
and device-side + cloud-side MCP tool control — still works, across the
~138 board directories XiaoZhi supports (see
[docs/supported-boards.md](docs/supported-boards.md)).

The part actively being built under the Yana Robot name is
**[`yana-wheelbot`](main/boards/yana-wheelbot)**, a from-scratch wheeled
robot board, plus **[`apps/controller-web`](apps/controller-web)**, a
browser control panel for it. Everything the board does — moving,
anti-fall sensing, LEDs, arm/neck servos, display — is exposed as an MCP
tool, controllable either locally over WebSocket or by a voice AI backend
speaking the same protocol (for example, the author's own
[Yana AI](https://github.com/yanacuti1121/Yana-AI) platform, via
`tools/yana-web/robot.js`).

**Honest status:** `yana-wheelbot` is build-verified only — it compiles
cleanly, and its GPIO defaults are cross-checked against a real published
reference design, but no physical unit has been built and run yet. See the
board's own README for the specific list of what still needs hardware
verification before any of it can be called solid.

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/wiring-diagram.svg" alt="Yana Wheelbot wiring diagram" width="480">

- Differential-drive movement, motor backend selectable at runtime
  (continuous-rotation servo pair or L298N DC driver)
- VL53L0X anti-fall sensor (VL6180X supported as a build option)
- Dual LEDs, arm + neck servos, TTP223 touch sensor
- Switchable display orientation/theme (ST7789 default, ST7735 as a build option)
- Full parts list, wiring diagram, and MCP tool reference:
  [main/boards/yana-wheelbot](main/boards/yana-wheelbot) ·
  [Tiếng Việt](main/boards/yana-wheelbot/README_vi.md) ·
  [한국어](main/boards/yana-wheelbot/README_ko.md)
- Browser control panel: [apps/controller-web](apps/controller-web)

## Origin

Yana Robot is built on top of the [XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32) project, the original open-source ESP32 voice-assistant firmware created by Xiaoqiang ([78](https://github.com/78)) and Shenzhen Xinzhi Future Technology Co., Ltd. Full credit for the original codebase, protocol design, and hardware ecosystem goes to that project and its contributors.

This repository is an independent continuation: it is not affiliated with, and does not track, the upstream project. From this point forward, boards, fixes, and features here are designed and maintained separately as Yana Robot.

## Requirements

- ESP-IDF v6.0 or later; v6.0.2 is the preferred stable SDK. v5.5.2 is
  retained only for documented legacy boards (mostly relevant to the
  inherited board set, not `yana-wheelbot`). See the
  [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md) for full
  compatibility and board-validation details.
- Cursor or VS Code with the ESP-IDF plugin. Linux compiles faster and has
  fewer driver issues than Windows.
- This project follows Google's C++ code style.

## Documentation

- [Yana Wheelbot board](main/boards/yana-wheelbot) — parts, wiring, MCP tools
- [Wheelbot web controller](apps/controller-web) — browser control panel
- [Custom Board Guide](docs/custom-board.md) — building a new board
- [MCP Protocol IoT Control Usage](docs/mcp-usage.md)
- [MCP Protocol Interaction Flow](docs/mcp-protocol.md)
- [WebSocket protocol](docs/websocket.md) · [MQTT + UDP protocol](docs/mqtt-udp.md)
- [Supported hardware](docs/supported-boards.md) — the full inherited board list, capability list, and compatible third-party ecosystem
- [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md)

The firmware connects to the official [xiaozhi.me](https://xiaozhi.me)
server by default if you don't point it at your own backend — personal
users can register an account there for free.

## About this project

Yana Robot is released under the MIT license, allowing anyone to use it for free, including for commercial purposes. The license file preserves the original copyright notice from the upstream XiaoZhi project, as required by that license.

The original firmware, protocol design, and board ecosystem were created by [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) and its contributors. Everything in this repository from this point forward — new boards, bug fixes, and feature work — is designed and maintained independently under the Yana Robot name, with no ongoing affiliation to the upstream project.

If you have any ideas or suggestions, please open an Issue on this repository.

## Star History

<a href="https://star-history.com/#yanacuti1121/yana-wheelbot&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
 </picture>
</a>
