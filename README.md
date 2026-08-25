# Yana Robot

(English | [Tiếng Việt](README_vi.md) | [한국어](README_ko.md))

ESP32-S3 voice-AI robotics platform: motor/servo control, anti-fall safety,
and MCP-based voice + web control, running on Yana's own fork of the
XiaoZhi AI Chatbot firmware base.

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/chassis/chassis-render.png" alt="Yana Wheelbot chassis, OpenSCAD render" width="480">
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
  [main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE.md](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE.md) ·
  [Tiếng Việt](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_vi.md) ·
  [한국어](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_ko.md)

**Honest status:** `yana-wheelbot` is build-verified only — it compiles
cleanly, and its GPIO defaults are cross-checked against a real published
reference design, but no physical unit has been built and run yet. See the
board's own README for the specific list of what still needs hardware
verification before any of it can be called solid.

## Control from a phone or computer

Yana Wheelbot does **not** currently serve a `/robot` web page itself. The
controller in [`apps/controller-web`](apps/controller-web) runs on a Mac,
Windows, or Linux computer and talks directly to the robot over the same
local Wi-Fi network.

### Before connecting

- Flash the `yana-wheelbot` firmware and connect the robot to a 2.4 GHz
  Wi-Fi network.
- Connect the controller computer and phone, if used, to the same LAN. Guest
  Wi-Fi/client isolation must be disabled.
- Find the robot's IP address in the router's DHCP/client list or ESP-IDF
  serial log.
- Obtain the six-digit local-control token through an already authenticated
  voice/cloud session using `self.local_control.get_token`.
- Install Node.js 18 or newer and npm on the computer hosting the web UI.

### Start the controller

```sh
cd apps/controller-web
npm install                 # first run only
npm run dev:lan
```

Vite prints both a **Local** and a **Network** address:

| Client | Address to open |
|---|---|
| Browser on the hosting computer | `http://localhost:5173` |
| iPhone, Android, or another computer | the printed Network URL, for example `http://192.168.1.20:5173` |

`localhost` on a phone means the phone itself, not the computer running the
controller. Allow the local-network/firewall prompt on macOS or Windows.

In the Yana Wheelbot page, enter:

- **Device URL:** `ws://<robot-ip>:8080/ws`
- **Pairing token:** the six-digit token obtained above

The page safely appends `?token=<code>` to the WebSocket handshake URL. Click
**Connect**, lift the wheels clear of the floor for the first test, verify
**STOP**, and only then test movement on the ground. The movement controls
use short renewable safety pulses; releasing a control, losing browser
focus, or disconnecting stops the motors.

### Connection troubleshooting

- **The Network URL does not open:** use `npm run dev:lan`, check the host
  firewall, and confirm both devices are on the same non-guest Wi-Fi.
- **The page opens but the robot does not connect:** verify the robot IP,
  port `8080`, token, and that AP/client isolation is off.
- **HTTP 401 or immediate WebSocket failure:** the token is missing or
  invalid. Retrieve it again; rotate it with
  `self.local_control.rotate_token` if it may have leaked.
- **An HTTPS-hosted page cannot open `ws://`:** browsers block mixed content.
  Use the local HTTP command above; the firmware does not currently provide
  `wss://`.
- Never expose port `8080` with router port forwarding. Local control is
  token-protected but unencrypted, so use it only on a trusted LAN.

### Protocol compatibility status

The source-level audit confirms that all 25 MCP tool names and argument
shapes used by the web UI match the firmware registrations. The handshake is
`ws://<robot-ip>:8080/ws?token=<code>` and the text frames use JSON-RPC 2.0
with MCP protocol version `2024-11-05`; the implemented methods are
`initialize`, `tools/list`, and `tools/call`.

This is the firmware's MCP-compatible JSON-RPC subset, not a general-purpose
MCP transport implementation. Responses are currently mirrored to every
authenticated local client, so use one local controller at a time to avoid
request-ID collisions. Voice and local commands can also overlap; the newest
movement command wins. These paths are source/build verified but still need
an end-to-end test on the physical robot.

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
