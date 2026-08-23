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
