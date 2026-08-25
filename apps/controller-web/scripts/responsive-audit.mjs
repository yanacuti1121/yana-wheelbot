import { spawn } from "node:child_process";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";

const chromePath =
  process.env.CHROME_PATH ?? "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome";
const controllerUrl = process.env.CONTROLLER_URL ?? "http://127.0.0.1:5173/";
const screenshotDirectory = process.env.AUDIT_SCREENSHOT_DIR;

const viewports = [
  { name: "iPhone SE", width: 375, height: 667, mobile: true, columns: 1 },
  { name: "iPhone modern", width: 390, height: 844, mobile: true, columns: 1 },
  { name: "Android", width: 412, height: 915, mobile: true, columns: 1 },
  { name: "Phone landscape", width: 844, height: 390, mobile: true, columns: 2 },
  { name: "iPad portrait", width: 820, height: 1180, mobile: true, columns: 2 },
  { name: "Windows laptop", width: 1366, height: 768, mobile: false, columns: 3 },
  { name: "Mac desktop", width: 1512, height: 982, mobile: false, columns: 3 },
];

const languages = ["en", "vi", "ko"];
const sleep = (milliseconds) => new Promise((resolve) => setTimeout(resolve, milliseconds));

async function waitForDevTools(profileDirectory) {
  const activePortFile = join(profileDirectory, "DevToolsActivePort");
  for (let attempt = 0; attempt < 100; attempt += 1) {
    try {
      const [port] = (await readFile(activePortFile, "utf8")).trim().split("\n");
      if (port) return Number(port);
    } catch {
      // Chrome creates the file asynchronously.
    }
    await sleep(50);
  }
  throw new Error("Chrome DevTools endpoint did not start");
}

function createCdpClient(webSocketUrl) {
  const socket = new WebSocket(webSocketUrl);
  const pending = new Map();
  let nextId = 1;

  const opened = new Promise((resolve, reject) => {
    socket.addEventListener("open", resolve, { once: true });
    socket.addEventListener("error", reject, { once: true });
  });

  socket.addEventListener("message", ({ data }) => {
    const message = JSON.parse(data);
    if (!message.id) return;
    const request = pending.get(message.id);
    if (!request) return;
    pending.delete(message.id);
    if (message.error) request.reject(new Error(message.error.message));
    else request.resolve(message.result);
  });

  return {
    async send(method, params = {}) {
      await opened;
      const id = nextId;
      nextId += 1;
      const result = new Promise((resolve, reject) => pending.set(id, { resolve, reject }));
      socket.send(JSON.stringify({ id, method, params }));
      return result;
    },
    close() {
      socket.close();
    },
  };
}

async function createPage(port) {
  const response = await fetch(`http://127.0.0.1:${port}/json/new?${encodeURIComponent(controllerUrl)}`, {
    method: "PUT",
  });
  if (!response.ok) throw new Error(`Unable to create Chrome page: ${response.status}`);
  return response.json();
}

function auditExpression(expectedColumns, mobile) {
  return `(() => {
    const selectorFor = (element) => {
      if (element.id) return '#' + element.id;
      const classes = [...element.classList].slice(0, 3).join('.');
      return element.tagName.toLowerCase() + (classes ? '.' + classes : '');
    };
    const viewportWidth = document.documentElement.clientWidth;
    const overflow = [...document.body.querySelectorAll('*')]
      .filter((element) => {
        const style = getComputedStyle(element);
        if (style.display === 'none' || style.position === 'fixed') return false;
        const rect = element.getBoundingClientRect();
        return rect.left < -1 || rect.right > viewportWidth + 1;
      })
      .slice(0, 12)
      .map((element) => {
        const rect = element.getBoundingClientRect();
        return { selector: selectorFor(element), left: rect.left, right: rect.right };
      });
    const dashboardColumns = getComputedStyle(document.querySelector('.dashboard'))
      .gridTemplateColumns.split(' ').filter(Boolean).length;
    const shortTargets = ${mobile}
      ? [...document.querySelectorAll('button, summary')]
          .filter((element) => element.getClientRects().length > 0)
          .map((element) => ({ selector: selectorFor(element), height: element.getBoundingClientRect().height }))
          .filter(({ height }) => height < 43.5)
      : [];
    const smallInputText = ${mobile}
      ? [...document.querySelectorAll('input:not([type="range"]), select')]
          .filter((element) => Number.parseFloat(getComputedStyle(element).fontSize) < 16)
          .map(selectorFor)
      : [];
    return {
      viewportWidth,
      innerWidth,
      scrollWidth: document.documentElement.scrollWidth,
      overflow,
      dashboardColumns,
      expectedColumns: ${expectedColumns},
      shortTargets,
      smallInputText,
    };
  })()`;
}

async function run() {
  const profileDirectory = await mkdtemp(join(tmpdir(), "yana-controller-chrome-"));
  const chrome = spawn(
    chromePath,
    [
      "--headless=new",
      "--disable-gpu",
      "--hide-scrollbars",
      "--no-first-run",
      "--no-default-browser-check",
      "--remote-debugging-port=0",
      `--user-data-dir=${profileDirectory}`,
      "about:blank",
    ],
    { stdio: "ignore" },
  );

  let client;
  try {
    const port = await waitForDevTools(profileDirectory);
    const page = await createPage(port);
    client = createCdpClient(page.webSocketDebuggerUrl);
    await client.send("Page.enable");
    await client.send("Runtime.enable");

    const failures = [];
    for (const viewport of viewports) {
      await client.send("Emulation.setDeviceMetricsOverride", {
        width: viewport.width,
        height: viewport.height,
        deviceScaleFactor: 1,
        mobile: viewport.mobile,
        screenWidth: viewport.width,
        screenHeight: viewport.height,
      });
      await client.send("Emulation.setTouchEmulationEnabled", {
        enabled: viewport.mobile,
        maxTouchPoints: viewport.mobile ? 5 : 1,
      });

      for (const language of languages) {
        await client.send("Page.navigate", { url: controllerUrl });
        await sleep(120);
        await client.send("Runtime.evaluate", {
          expression: `localStorage.setItem('yana-wheelbot-lang', '${language}')`,
        });
        await client.send("Page.reload", { ignoreCache: true });
        await sleep(180);

        const response = await client.send("Runtime.evaluate", {
          expression: auditExpression(viewport.columns, viewport.mobile),
          returnByValue: true,
        });
        const result = response.result.value;
        const failed =
          result.scrollWidth > result.viewportWidth + 1 ||
          result.overflow.length > 0 ||
          result.dashboardColumns !== viewport.columns ||
          result.shortTargets.length > 0 ||
          result.smallInputText.length > 0;
        const label = `${viewport.name.padEnd(17)} ${language}`;
        console.log(
          `${failed ? "FAIL" : "PASS"}  ${label}  ${viewport.width}x${viewport.height}  ` +
            `grid:${result.dashboardColumns}  width:${result.viewportWidth}/${result.scrollWidth}`,
        );
        if (failed) failures.push({ viewport: viewport.name, language, ...result });

        if (screenshotDirectory && language === "vi") {
          await mkdir(screenshotDirectory, { recursive: true });
          const screenshot = await client.send("Page.captureScreenshot", {
            format: "png",
            fromSurface: true,
            captureBeyondViewport: false,
          });
          const filename = viewport.name.toLowerCase().replaceAll(/[^a-z0-9]+/g, "-");
          await writeFile(join(screenshotDirectory, `${filename}.png`), screenshot.data, "base64");
        }
      }
    }

    if (failures.length) {
      console.error(JSON.stringify(failures, null, 2));
      process.exitCode = 1;
    }
  } finally {
    client?.close();
    const chromeExited = new Promise((resolve) => chrome.once("exit", resolve));
    chrome.kill("SIGTERM");
    await Promise.race([chromeExited, sleep(2000)]);
    await sleep(250);
    try {
      await rm(profileDirectory, { recursive: true, force: true, maxRetries: 5, retryDelay: 100 });
    } catch (error) {
      console.warn(`Unable to remove temporary Chrome profile: ${error.message}`);
    }
  }
}

await run();
