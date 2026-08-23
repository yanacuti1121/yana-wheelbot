// Thin wrapper around the WebSocket control server implemented in
// main/boards/yana-wheelbot/websocket_control_server.cc (copied from
// main/boards/otto-robot, board-agnostic). Speaks the JSON-RPC-2.0-style
// envelope documented in main/boards/otto-robot/README.md's "WebSocket
// 直连调试接口" section: {"jsonrpc":"2.0","method":"tools/call","params":
// {"name":...,"arguments":{...}},"id":N}. The server forwards this straight
// into McpServer::ParseMessage() -- see websocket_control_server.cc's
// HandleMessage() -- so this is the same protocol the cloud channel speaks,
// just over a local LAN WebSocket instead of MQTT/WebSocket-to-cloud.

export type ToolArguments = Record<string, string | number | boolean>;

type PendingCall = {
  resolve: (value: unknown) => void;
  reject: (reason: unknown) => void;
  timeoutId: number;
};

export type BroadcastListener = (payload: unknown) => void;

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === "object" && value !== null;
}

// How long to wait for a response to a single tools/call before giving up.
// Without this, a request sent right before the link drops (Wi-Fi loss,
// device reboot) would sit in `pending` forever -- see rejectAllPending()
// for the close/error-path half of the same fix.
const CALL_TIMEOUT_MS = 8000;

export class McpClient {
  private ws: WebSocket | null = null;
  private nextId = 1;
  private pending = new Map<number, PendingCall>();
  private broadcastListeners: BroadcastListener[] = [];
  private onStatusChange: (connected: boolean) => void;
  // Bumped on every connect()/disconnect() so a socket's own onopen/onclose/
  // onerror/onmessage callbacks can tell if they're still the current
  // connection before touching shared state -- prevents a slow-to-fail
  // earlier socket (e.g. from a double-click on Connect while the first
  // attempt was still CONNECTING) from clobbering the status/pending state
  // of a newer, already-open one.
  private generation = 0;

  constructor(onStatusChange: (connected: boolean) => void) {
    this.onStatusChange = onStatusChange;
  }

  connect(deviceUrl: string) {
    if (this.isConnected || this.isConnecting) return;

    const generation = ++this.generation;
    const socket = new WebSocket(deviceUrl);
    this.ws = socket;

    socket.onopen = () => {
      if (this.generation !== generation) return;
      this.onStatusChange(true);
      this.send("initialize", {
        protocolVersion: "2024-11-05",
        capabilities: {},
      });
      this.send("tools/list", {});
    };

    socket.onclose = () => {
      if (this.generation !== generation) return;
      this.rejectAllPending(new Error("WebSocket disconnected"));
      this.onStatusChange(false);
    };

    socket.onerror = () => {
      if (this.generation !== generation) return;
      this.rejectAllPending(new Error("WebSocket error"));
      this.onStatusChange(false);
    };

    socket.onmessage = (event) => {
      if (this.generation !== generation) return;
      this.handleMessage(event.data);
    };
  }

  disconnect() {
    this.generation++;
    this.ws?.close();
    this.ws = null;
    this.rejectAllPending(new Error("Disconnected"));
  }

  get isConnected(): boolean {
    return this.ws !== null && this.ws.readyState === WebSocket.OPEN;
  }

  get isConnecting(): boolean {
    return this.ws !== null && this.ws.readyState === WebSocket.CONNECTING;
  }

  onBroadcast(listener: BroadcastListener) {
    this.broadcastListeners.push(listener);
  }

  callTool(name: string, args: ToolArguments = {}): Promise<unknown> {
    return this.send("tools/call", { name, arguments: args });
  }

  private rejectAllPending(reason: unknown) {
    for (const call of this.pending.values()) {
      window.clearTimeout(call.timeoutId);
      call.reject(reason);
    }
    this.pending.clear();
  }

  private send(method: string, params: unknown): Promise<unknown> {
    if (!this.isConnected) {
      return Promise.reject(new Error("Not connected"));
    }
    const id = this.nextId++;
    const envelope = { jsonrpc: "2.0", method, params, id };
    return new Promise((resolve, reject) => {
      const timeoutId = window.setTimeout(() => {
        this.pending.delete(id);
        reject(new Error(`RPC timed out: ${method}`));
      }, CALL_TIMEOUT_MS);
      this.pending.set(id, { resolve, reject, timeoutId });
      this.ws!.send(JSON.stringify(envelope));
    });
  }

  private handleMessage(raw: string) {
    let msg: unknown;
    try {
      msg = JSON.parse(raw);
    } catch {
      return;
    }
    if (!isRecord(msg)) return;

    const id = msg["id"];
    if (typeof id === "number" && this.pending.has(id)) {
      const call = this.pending.get(id)!;
      this.pending.delete(id);
      window.clearTimeout(call.timeoutId);
      if (msg["error"] !== undefined && msg["error"] !== null) {
        call.reject(msg["error"]);
      } else {
        call.resolve(msg["result"]);
      }
      return;
    }

    // Unsolicited frame (e.g. a voice-triggered MCP response mirrored via
    // Application::RegisterMcpBroadcastCallback in yana_wheelbot_board.cc) --
    // hand it to anyone listening so the UI can reflect state it didn't
    // itself request.
    for (const listener of this.broadcastListeners) {
      listener(msg);
    }
  }
}
