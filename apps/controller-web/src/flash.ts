// Browser-based firmware flashing over Web Serial, using esptool-js.
// Flashes a single pre-merged image (idf.py merge-bin output) at offset 0x0 --
// this is not a general esptool replacement, see README's Known limitation.

import { ESPLoader, Transport, type FlashOptions, type IEspLoaderTerminal } from "esptool-js";
import { applyStaticTranslations, getLang, setLang, t, type Lang } from "./i18n";

const MERGED_IMAGE_FLASH_ADDRESS = 0x0;
const DEFAULT_BAUDRATE = 115200;

const supportWarningEl = document.getElementById("support-warning") as HTMLElement;
const connectBtn = document.getElementById("connect-btn") as HTMLButtonElement;
const chipInfoEl = document.getElementById("chip-info") as HTMLElement;
const fileInput = document.getElementById("firmware-file") as HTMLInputElement;
const fileStatusEl = document.getElementById("file-status") as HTMLElement;
const flashBtn = document.getElementById("flash-btn") as HTMLButtonElement;
const progressBarEl = document.getElementById("progress-bar") as HTMLProgressElement;
const logEl = document.getElementById("log") as HTMLPreElement;
const langSelect = document.getElementById("lang-select") as HTMLSelectElement;

let esploader: ESPLoader | null = null;
let transport: Transport | null = null;
let firmwareBytes: Uint8Array | null = null;

function appendLog(line: string): void {
  logEl.textContent += `${line}\n`;
  logEl.scrollTop = logEl.scrollHeight;
}

const terminal: IEspLoaderTerminal = {
  clean() {
    logEl.textContent = "";
  },
  writeLine(data: string) {
    appendLog(data);
  },
  write(data: string) {
    logEl.textContent += data;
    logEl.scrollTop = logEl.scrollHeight;
  },
};

function updateFlashButtonState(): void {
  flashBtn.disabled = esploader === null || firmwareBytes === null;
}

async function handleConnect(): Promise<void> {
  connectBtn.disabled = true;
  try {
    const port = await navigator.serial.requestPort();
    transport = new Transport(port, true);
    esploader = new ESPLoader({ transport, baudrate: DEFAULT_BAUDRATE, terminal });
    const chipName = await esploader.main();
    chipInfoEl.textContent = t("flashConnectedTo", { chip: chipName });
  } catch (err) {
    appendLog(`${t("flashConnectFailed")}: ${(err as Error).message}`);
    esploader = null;
    transport = null;
  } finally {
    connectBtn.disabled = false;
    updateFlashButtonState();
  }
}

async function handleFileSelected(): Promise<void> {
  const file = fileInput.files?.[0] ?? null;
  if (!file) {
    firmwareBytes = null;
    fileStatusEl.textContent = t("flashNoFile");
    updateFlashButtonState();
    return;
  }
  const buffer = await file.arrayBuffer();
  firmwareBytes = new Uint8Array(buffer);
  fileStatusEl.textContent = t("flashFileLoaded", { name: file.name, size: String(firmwareBytes.length) });
  updateFlashButtonState();
}

async function handleFlash(): Promise<void> {
  if (esploader === null || firmwareBytes === null) return;
  connectBtn.disabled = true;
  flashBtn.disabled = true;
  fileInput.disabled = true;
  progressBarEl.hidden = false;
  progressBarEl.value = 0;
  appendLog(t("flashInProgress"));
  try {
    const flashOptions: FlashOptions = {
      fileArray: [{ data: firmwareBytes, address: MERGED_IMAGE_FLASH_ADDRESS }],
      flashMode: "keep",
      flashFreq: "keep",
      flashSize: "keep",
      eraseAll: false,
      compress: true,
      reportProgress(_fileIndex: number, written: number, total: number) {
        progressBarEl.value = total > 0 ? (written / total) * 100 : 0;
      },
    };
    await esploader.writeFlash(flashOptions);
    await esploader.after("hard_reset");
    appendLog(t("flashDone"));
  } catch (err) {
    appendLog(`${t("flashFailed")}: ${(err as Error).message}`);
  } finally {
    connectBtn.disabled = false;
    fileInput.disabled = false;
    updateFlashButtonState();
  }
}

function checkWebSerialSupport(): boolean {
  const supported = "serial" in navigator;
  supportWarningEl.hidden = supported;
  connectBtn.disabled = !supported;
  return supported;
}

connectBtn.addEventListener("click", () => void handleConnect());
flashBtn.addEventListener("click", () => void handleFlash());
fileInput.addEventListener("change", () => void handleFileSelected());
langSelect.addEventListener("change", () => setLang(langSelect.value as Lang));

window.addEventListener("beforeunload", () => {
  void transport?.disconnect();
});

checkWebSerialSupport();
langSelect.value = getLang();
applyStaticTranslations();
