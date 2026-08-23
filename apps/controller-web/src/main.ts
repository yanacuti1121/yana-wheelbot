import { McpClient } from "./mcp-client";
import { Tools } from "./tools";
import { applyStaticTranslations, getLang, setLang, t, type Lang } from "./i18n";

const $ = <T extends HTMLElement>(id: string) => document.getElementById(id) as T;

applyStaticTranslations();

const langSelectEl = $<HTMLSelectElement>("lang-select");
langSelectEl.value = getLang();
langSelectEl.addEventListener("change", () => {
  setLang(langSelectEl.value as Lang);
  // Re-apply whatever the current connection/drive state is under the new
  // language -- the static pass above only covers markup with a data-i18n
  // attribute, not text this file already set dynamically (e.g. "Driving
  // forward" or "SAFE"/"ALERT").
  statusEl.textContent = client.isConnected ? t("statusConnected") : t("statusDisconnected");
  driveStatusEl.textContent = activeDrive
    ? t("driving", { direction: t(`direction${capitalize(activeDrive.direction)}`) })
    : t("stopped");
});

function capitalize(s: string): string {
  return s.charAt(0).toUpperCase() + s.slice(1);
}

// The board's local WebSocket control endpoint requires a pairing token as
// a query param (see websocket_control_server.cc's IsAuthorized()). Get it
// from the device via voice/cloud MCP: self.local_control.get_token.
function withToken(deviceUrl: string, token: string): string {
  if (!token) return deviceUrl;
  try {
    const url = new URL(deviceUrl);
    url.searchParams.set("token", token);
    return url.toString();
  } catch {
    const separator = deviceUrl.includes("?") ? "&" : "?";
    return `${deviceUrl}${separator}token=${encodeURIComponent(token)}`;
  }
}

const statusEl = $<HTMLSpanElement>("status");
const statusCliffEl = $<HTMLSpanElement>("status-cliff");
const statusMotionEl = $<HTMLSpanElement>("status-motion");
const client = new McpClient((connected) => {
  statusEl.textContent = connected ? t("statusConnected") : t("statusDisconnected");
  statusEl.className = `status-pill ${connected ? "connected" : "disconnected"}`;
  if (!connected) {
    stopDriving(false);
    statusCliffEl.textContent = t("antiFallDash");
    statusCliffEl.className = "status-pill status-pill-quiet";
    statusMotionEl.textContent = t("motorsDash");
    statusMotionEl.className = "status-pill status-pill-quiet";
    setCliffBadge(null);
    stopStatusPolling();
  } else {
    startStatusPolling();
  }
});

client.onBroadcast((payload) => {
  // Voice-triggered changes (e.g. someone says "turn on the lights") get
  // mirrored here too; for this simple controller we just log them --
  // extend this to refresh specific panel state if desired.
  console.log("[broadcast]", payload);
});

$<HTMLButtonElement>("connect-btn").addEventListener("click", () => {
  if (client.isConnected) {
    client.disconnect();
    return;
  }
  const url = $<HTMLInputElement>("device-url").value.trim();
  const token = $<HTMLInputElement>("device-token").value.trim();
  if (!url) return;
  client.connect(withToken(url, token));
});

type DriveDirection = "forward" | "backward" | "left" | "right";

const driveTools: Record<DriveDirection, string> = {
  forward: Tools.moveForward,
  backward: Tools.moveBackward,
  left: Tools.turnLeft,
  right: Tools.turnRight,
};
const driveButtons: Record<DriveDirection, HTMLButtonElement> = {
  forward: $<HTMLButtonElement>("btn-forward"),
  backward: $<HTMLButtonElement>("btn-backward"),
  left: $<HTMLButtonElement>("btn-left"),
  right: $<HTMLButtonElement>("btn-right"),
};
const driveStatusEl = $<HTMLSpanElement>("drive-status");
const gamepadStatusEl = $<HTMLSpanElement>("gamepad-status");

let activeDrive: { direction: DriveDirection; source: string } | null = null;
let heartbeatTimer: number | null = null;

function safetyPulseMs() {
  const input = $<HTMLInputElement>("duration-ms");
  const value = Math.round(Number(input.value));
  const clamped = Number.isFinite(value) ? Math.min(2000, Math.max(250, value)) : 750;
  input.value = String(clamped);
  return clamped;
}

function driveSpeed() {
  const input = $<HTMLInputElement>("speed");
  const value = Math.round(Number(input.value));
  const clamped = Number.isFinite(value) ? Math.min(100, Math.max(0, value)) : 80;
  input.value = String(clamped);
  return clamped;
}

function callToolSafely(name: string, args: Record<string, string | number | boolean> = {}) {
  void client.callTool(name, args).catch((error) => {
    console.error(`[control] ${name} failed`, error);
    driveStatusEl.textContent = t("commandFailed");
  });
}

function sendDrivePulse() {
  if (!activeDrive || !client.isConnected) return;
  callToolSafely(driveTools[activeDrive.direction], {
    duration_ms: safetyPulseMs(),
    speed: driveSpeed(),
  });
}

function startDriving(direction: DriveDirection, source: string) {
  if (!client.isConnected) {
    driveStatusEl.textContent = t("connectFirst");
    return;
  }
  if (activeDrive?.direction === direction && activeDrive.source === source) return;

  if (activeDrive) {
    callToolSafely(Tools.stop);
    driveButtons[activeDrive.direction].classList.remove("active");
  }

  activeDrive = { direction, source };
  driveButtons[direction].classList.add("active");
  driveStatusEl.textContent = t("driving", { direction: t(`direction${capitalize(direction)}`) });
  sendDrivePulse();

  if (heartbeatTimer !== null) window.clearInterval(heartbeatTimer);
  const heartbeatMs = Math.max(150, Math.floor(safetyPulseMs() / 2));
  heartbeatTimer = window.setInterval(sendDrivePulse, heartbeatMs);
}

function stopDriving(sendStop = true, source?: string) {
  if (source && activeDrive?.source !== source) return;

  if (heartbeatTimer !== null) {
    window.clearInterval(heartbeatTimer);
    heartbeatTimer = null;
  }
  if (activeDrive) {
    driveButtons[activeDrive.direction].classList.remove("active");
  }
  activeDrive = null;
  driveStatusEl.textContent = t("stopped");
  if (sendStop && client.isConnected) callToolSafely(Tools.stop);
}

for (const [direction, button] of Object.entries(driveButtons) as [
  DriveDirection,
  HTMLButtonElement,
][]) {
  const source = `pointer:${direction}`;
  button.addEventListener("pointerdown", (event) => {
    event.preventDefault();
    button.setPointerCapture(event.pointerId);
    startDriving(direction, source);
  });
  button.addEventListener("pointerup", () => stopDriving(true, source));
  button.addEventListener("pointercancel", () => stopDriving(true, source));
  button.addEventListener("lostpointercapture", () => stopDriving(true, source));
  button.addEventListener("contextmenu", (event) => event.preventDefault());
}

$<HTMLButtonElement>("btn-stop").addEventListener("click", () => stopDriving());

const keyDirections: Record<string, DriveDirection> = {
  w: "forward",
  arrowup: "forward",
  s: "backward",
  arrowdown: "backward",
  a: "left",
  arrowleft: "left",
  d: "right",
  arrowright: "right",
};
const pressedDriveKeys: string[] = [];

function isTypingTarget(target: EventTarget | null) {
  return (
    target instanceof HTMLInputElement ||
    target instanceof HTMLSelectElement ||
    target instanceof HTMLTextAreaElement
  );
}

window.addEventListener("keydown", (event) => {
  if (isTypingTarget(event.target)) return;
  const key = event.key.toLowerCase();
  const direction = keyDirections[key];
  if (!direction) return;
  event.preventDefault();
  if (!pressedDriveKeys.includes(key)) pressedDriveKeys.push(key);
  startDriving(direction, `keyboard:${key}`);
});

window.addEventListener("keyup", (event) => {
  const key = event.key.toLowerCase();
  if (!keyDirections[key]) return;
  event.preventDefault();
  const index = pressedDriveKeys.indexOf(key);
  if (index >= 0) pressedDriveKeys.splice(index, 1);

  if (activeDrive?.source === `keyboard:${key}`) {
    const fallbackKey = pressedDriveKeys[pressedDriveKeys.length - 1];
    if (fallbackKey) {
      startDriving(keyDirections[fallbackKey], `keyboard:${fallbackKey}`);
    } else {
      stopDriving();
    }
  }
});

function gamepadDirection(gamepad: Gamepad): DriveDirection | null {
  const dpad: [number, DriveDirection][] = [
    [12, "forward"],
    [13, "backward"],
    [14, "left"],
    [15, "right"],
  ];
  for (const [button, direction] of dpad) {
    if (gamepad.buttons[button]?.pressed) return direction;
  }

  const x = gamepad.axes[0] ?? 0;
  const y = gamepad.axes[1] ?? 0;
  const deadZone = 0.35;
  if (Math.abs(x) < deadZone && Math.abs(y) < deadZone) return null;
  if (Math.abs(y) >= Math.abs(x)) return y < 0 ? "forward" : "backward";
  return x < 0 ? "left" : "right";
}

function pollGamepads() {
  const gamepads = typeof navigator.getGamepads === "function" ? navigator.getGamepads() : [];
  const gamepad = Array.from(gamepads).find((candidate) => candidate?.connected);
  if (gamepad) {
    gamepadStatusEl.textContent = t("gamepadConnected", { id: gamepad.id });
    const source = `gamepad:${gamepad.index}`;
    const direction = gamepadDirection(gamepad);
    if (direction) startDriving(direction, source);
    else stopDriving(true, source);
  } else {
    gamepadStatusEl.textContent = t("noGamepad");
    if (activeDrive?.source.startsWith("gamepad:")) stopDriving();
  }
  window.requestAnimationFrame(pollGamepads);
}

window.addEventListener("blur", () => stopDriving());
window.addEventListener("pagehide", () => stopDriving());
document.addEventListener("visibilitychange", () => {
  if (document.hidden) stopDriving();
});
window.addEventListener("gamepaddisconnected", () => stopDriving());
window.requestAnimationFrame(pollGamepads);

$<HTMLButtonElement>("motor-type-apply").addEventListener("click", () => {
  const type = $<HTMLSelectElement>("motor-type").value;
  client.callTool(Tools.setMotorType, { type });
});

$<HTMLButtonElement>("pins-apply").addEventListener("click", () => {
  client.callTool(Tools.setMotorPins, {
    in1: Number($<HTMLInputElement>("pin-in1").value),
    in2: Number($<HTMLInputElement>("pin-in2").value),
    in3: Number($<HTMLInputElement>("pin-in3").value),
    in4: Number($<HTMLInputElement>("pin-in4").value),
  });
});

$<HTMLButtonElement>("stop-pulse-apply").addEventListener("click", () => {
  client.callTool(Tools.setServoStopPulse, {
    microseconds: Number($<HTMLInputElement>("stop-pulse").value),
  });
});

$<HTMLInputElement>("rev-left").addEventListener("change", (e) => {
  client.callTool(Tools.setServoReverse, {
    side: "left",
    reversed: (e.target as HTMLInputElement).checked,
  });
});
$<HTMLInputElement>("rev-right").addEventListener("change", (e) => {
  client.callTool(Tools.setServoReverse, {
    side: "right",
    reversed: (e.target as HTMLInputElement).checked,
  });
});

$<HTMLButtonElement>("refresh-motor-config").addEventListener("click", async () => {
  const config = await client.callTool(Tools.getMotorConfig);
  $<HTMLPreElement>("motor-config-out").textContent = JSON.stringify(config, null, 2);
});

$<HTMLButtonElement>("cliff-apply").addEventListener("click", () => {
  client.callTool(Tools.cliffSetEnabled, {
    enabled: $<HTMLInputElement>("cliff-enabled").checked,
  });
  client.callTool(Tools.cliffSetThreshold, {
    threshold_mm: Number($<HTMLInputElement>("cliff-threshold").value),
  });
});

const cliffStateEl = $<HTMLSpanElement>("cliff-state");
const tofDistanceEl = $<HTMLSpanElement>("tof-distance");
const controlSourceEl = $<HTMLSpanElement>("control-source");

// mm < 0 (sensor error) or mm >= threshold means "no floor confirmed" --
// same fail-safe semantics as cliff_sensor_controller.cc's own PollTask,
// see that file's header comment for why the polarity is ">= threshold".
function setCliffBadge(state: "safe" | "alert" | null) {
  if (state === null) {
    cliffStateEl.textContent = t("dash");
    cliffStateEl.className = "badge badge-quiet";
    return;
  }
  cliffStateEl.textContent = state === "safe" ? t("safe") : t("alert");
  cliffStateEl.className = `badge ${state === "safe" ? "badge-safe" : "badge-alert"}`;
  statusCliffEl.textContent = state === "safe" ? t("antiFallOk") : t("antiFallAlert");
  statusCliffEl.className = `status-pill ${state === "safe" ? "status-pill-quiet" : "disconnected"}`;
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === "object" && value !== null;
}

let statusPollTimer: number | null = null;

function startStatusPolling() {
  if (statusPollTimer !== null) return;
  const poll = async () => {
    if (!client.isConnected) return;
    try {
      const [cliffConfig, distance, controlStatus] = await Promise.all([
        client.callTool(Tools.cliffGetConfig),
        client.callTool(Tools.cliffTestNow),
        client.callTool(Tools.getControlStatus),
      ]);

      if (isRecord(cliffConfig) && typeof distance === "number") {
        const threshold = Number(cliffConfig["threshold_mm"] ?? 50);
        const enabled = Boolean(cliffConfig["enabled"]);
        tofDistanceEl.textContent = `${distance} mm`;
        if (!enabled) {
          setCliffBadge(null);
        } else {
          setCliffBadge(distance < 0 || distance >= threshold ? "alert" : "safe");
        }
      }

      if (isRecord(controlStatus)) {
        const isMoving = Boolean(controlStatus["is_moving"]);
        const lastAction = String(controlStatus["last_action"] ?? "none");
        const lastSource = String(controlStatus["last_action_source"] ?? "—");
        statusMotionEl.textContent = isMoving ? t("motorsAction", { action: lastAction }) : t("motorsIdle");
        statusMotionEl.className = `status-pill ${isMoving ? "connected" : "status-pill-quiet"}`;
        controlSourceEl.textContent = lastSource;
      }
    } catch (error) {
      console.error("[status] poll failed", error);
    }
  };
  void poll();
  statusPollTimer = window.setInterval(poll, 2000);
}

function stopStatusPolling() {
  if (statusPollTimer !== null) {
    window.clearInterval(statusPollTimer);
    statusPollTimer = null;
  }
}

$<HTMLButtonElement>("cliff-test").addEventListener("click", async () => {
  const distance = await client.callTool(Tools.cliffTestNow);
  if (typeof distance === "number") tofDistanceEl.textContent = `${distance} mm`;
});

document.querySelectorAll<HTMLButtonElement>("[data-mode]").forEach((btn) => {
  btn.addEventListener("click", () => {
    client.callTool(Tools.ledSetMode, { mode: btn.dataset.mode! });
  });
});

const armAngleValueEl = $<HTMLOutputElement>("arm-angle-value");
$<HTMLInputElement>("arm-angle").addEventListener("input", (e) => {
  const angle = Number((e.target as HTMLInputElement).value);
  armAngleValueEl.textContent = `${angle}°`;
  client.callTool(Tools.armSetAngle, { angle });
});
$<HTMLButtonElement>("arm-wave").addEventListener("click", () => client.callTool(Tools.armWave));
$<HTMLButtonElement>("arm-release").addEventListener("click", () =>
  client.callTool(Tools.armRelease),
);

const neckAngleValueEl = $<HTMLOutputElement>("neck-angle-value");
$<HTMLInputElement>("neck-angle").addEventListener("input", (e) => {
  const angle = Number((e.target as HTMLInputElement).value);
  neckAngleValueEl.textContent = `${angle}°`;
  client.callTool(Tools.neckSetAngle, { angle });
});
$<HTMLButtonElement>("neck-left").addEventListener("click", () =>
  client.callTool(Tools.neckTurn, { direction: "left" }),
);
$<HTMLButtonElement>("neck-center").addEventListener("click", () =>
  client.callTool(Tools.neckTurn, { direction: "center" }),
);
$<HTMLButtonElement>("neck-right").addEventListener("click", () =>
  client.callTool(Tools.neckTurn, { direction: "right" }),
);
$<HTMLButtonElement>("neck-release").addEventListener("click", () =>
  client.callTool(Tools.neckRelease),
);

$<HTMLButtonElement>("theme-apply").addEventListener("click", () => {
  client.callTool(Tools.screenSetTheme, { theme: $<HTMLSelectElement>("theme").value });
});

$<HTMLButtonElement>("orientation-apply").addEventListener("click", () => {
  client.callTool(Tools.screenSetOrientation, {
    orientation: $<HTMLSelectElement>("orientation").value,
  });
});
