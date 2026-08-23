# Yana Robot

(English | [Tiếng Việt](README_vi.md) | [한국어](README_ko.md))

ESP32-S3 voice-AI robotics platform: motor/servo control, anti-fall safety,
and MCP-based voice + web control, running on Yana's own fork of the
XiaoZhi AI Chatbot firmware base.

<img src="docs/mcp-based-graph.jpg" alt="Control everything via MCP" width="320">

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/wiring-diagram.svg" alt="Yana Wheelbot wiring diagram" width="480">

- Differential-drive movement, motor backend selectable at runtime
  (continuous-rotation servo pair or L298N DC driver)
- VL53L0X anti-fall sensor (VL6180X supported as a build option), with a
  movement-inhibit latch so an unsafe reading actually blocks the next move
- Dual LEDs, arm + neck servos, TTP223 touch sensor
- Switchable display orientation/theme (ST7789 default, ST7735 as a build option)
- Full parts list, wiring diagram, and MCP tool reference:
  [main/boards/yana-wheelbot](main/boards/yana-wheelbot) ·
  [Tiếng Việt](main/boards/yana-wheelbot/README_vi.md) ·
  [한국어](main/boards/yana-wheelbot/README_ko.md)
- Browser control panel, localized EN/VI/KO: [apps/controller-web](apps/controller-web)
- Printable chassis (BOM, wiring, assembly guide):
  [main/boards/yana-wheelbot/chassis](main/boards/yana-wheelbot/chassis)

**Honest status:** `yana-wheelbot` is build-verified only — it compiles
cleanly, and its GPIO defaults are cross-checked against a real published
reference design, but no physical unit has been built and run yet. See the
board's own README for the specific list of what still needs hardware
verification before any of it can be called solid.

## Requirements

- ESP-IDF v6.0 or later; v6.0.2 is the preferred stable SDK. See the
  [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md) for full
  compatibility details.
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
- [Supported hardware](docs/supported-boards.md)
- [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md)

The firmware connects to the official [xiaozhi.me](https://xiaozhi.me)
server by default if you don't point it at your own backend — personal
users can register an account there for free.

## License and origin

Yana Robot is released under the MIT license — free to use, including
commercially. It's built on top of [XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32)
(also MIT), created by Xiaoqiang ([78](https://github.com/78)) and Shenzhen
Xinzhi Future Technology Co., Ltd.; the LICENSE file preserves their
original copyright notice as the license requires. This repository is an
independent continuation, not affiliated with or tracking the upstream
project — see [docs/origin-story.md](docs/origin-story.md) for the full
writeup of what's inherited versus newly built.

If you have any ideas or suggestions, please open an Issue on this repository.

## Star History

<a href="https://star-history.com/#yanacuti1121/yana-wheelbot&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
 </picture>
</a>
