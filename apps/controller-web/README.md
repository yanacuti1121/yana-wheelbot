# Yana Wheelbot Controller (web)

A browser-based control panel for the `yana-wheelbot` board
(`main/boards/yana-wheelbot`). Connects directly to the device over the local
network via the same WebSocket + MCP protocol the board's local control
server speaks (see that board's `README.md`), so there is no cloud
round-trip and no backend server to run.

## Run it

```sh
npm install
npm run dev
```

To open the controller from an iPhone, Android device, or another computer
on the same Wi-Fi, expose the development server to the local network:

```sh
npm run dev:lan
```

Open the printed **Network** URL (for example `http://192.168.1.20:5173`) on
the other device. Do not use `localhost` there: it refers to that phone or
computer, not to the machine running Vite. A Mac or Windows firewall may ask
for permission to accept local-network connections.

Open the printed local URL, type `ws://<device-ip>:8080/ws` into the
connection bar, enter the device's pairing token (get it via voice/cloud
control by asking for `self.local_control.get_token`) in the Token field,
and click Connect. The device must be on the same network and already
connected to Wi-Fi.

## Dead-man movement controls

Movement is hold-to-run: hold a direction on the on-screen D-pad, WASD,
arrow keys, a connected gamepad's D-pad, or its left stick. Releasing the
control immediately sends `self.wheelbot.stop`. Losing window focus, hiding
the tab, navigating away, or disconnecting a gamepad also stops movement.

While held, the controller renews a short movement pulse. The firmware stops
at the end of that pulse if the browser freezes or the connection disappears.
The **Safety pulse** field controls its duration; keep it short enough for the
robot's speed and environment.

## Build for static hosting

```sh
npm run build
```

Output goes to `dist/` — a static bundle, servable from any static host or
opened locally.

## Responsive layouts

The controller uses separate layouts instead of shrinking the desktop UI:

- Phones up to 700 px use one column, large touch controls, and a stacked
  connection form.
- Tablets and landscape phones use two columns so movement stays visible
  beside the secondary controls.
- Mac and Windows desktops use a three-column dashboard capped at 1440 px.

Notched-phone safe areas are respected, touch targets are at least 44 px,
and text inputs remain at 16 px on touch devices to avoid unwanted Safari
zoom. The UI uses system fonts so the local controller does not depend on an
internet font service.

With the development server running, audit the supported viewport matrix in
an installed Google Chrome:

```sh
npm run audit:responsive
```

Set `CONTROLLER_URL`, `CHROME_PATH`, or `AUDIT_SCREENSHOT_DIR` to override
the defaults or capture screenshots. The audit covers common iPhone,
Android, iPad, Mac, and Windows viewport sizes in English, Vietnamese, and
Korean.

## Structure

- `src/mcp-client.ts` — WebSocket wrapper speaking the JSON-RPC-2.0-style
  envelope the firmware's `WebSocketControlServer` expects.
- `src/tools.ts` — the single source of truth for every MCP tool name and
  argument shape this app calls; keep this in sync with the firmware side
  (`main/boards/yana-wheelbot/*_controller.cc`).
- `src/main.ts` — wires the DOM controls in `index.html` to `mcp-client.ts`
  calls.

## Known limitation

This is a minimal reference client, not a production app: it does not
persist connection history, does not retry on disconnect, and its broadcast
handler (for voice-triggered state changes) only logs to the console instead
of refreshing specific panels. Extend as needed.
