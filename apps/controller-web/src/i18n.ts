// Minimal i18n: English/Vietnamese/Korean, matching this repo's existing
// README.md/README_vi.md/README_ko.md language convention. No framework --
// data-i18n attributes on static markup, t() for strings set from main.ts.

export type Lang = "en" | "vi" | "ko";

const STORAGE_KEY = "yana-wheelbot-lang";

type Dict = Record<string, string>;

const en: Dict = {
  deviceUrlPlaceholder: "ws://192.168.1.50:8080/ws",
  tokenPlaceholder: "Pairing token",
  connect: "Connect",
  disconnect: "Disconnect",
  statusConnected: "● Connected",
  statusDisconnected: "● Disconnected",
  antiFallDash: "Anti-fall —",
  antiFallOk: "Anti-fall OK",
  antiFallAlert: "Anti-fall ALERT",
  motorsDash: "Motors —",
  motorsIdle: "Motors: idle",
  motorsAction: "Motors: {action}",

  movementTitle: "Movement",
  safetyPulse: "Safety pulse (ms)",
  speed: "Speed",
  movementHelp: "Hold a direction to move; release to stop. Keyboard: WASD or arrow keys.",
  stopped: "Stopped",
  driving: "Driving {direction}",
  connectFirst: "Connect before driving",
  commandFailed: "Command failed",
  noGamepad: "No gamepad",
  gamepadConnected: "Gamepad: {id}",
  directionForward: "forward",
  directionBackward: "backward",
  directionLeft: "left",
  directionRight: "right",

  armNeckTitle: "Arm & Neck",
  arm: "Arm",
  neck: "Neck",
  wave: "Wave",
  releaseArm: "Release arm",
  turnLeft: "Turn left",
  center: "Center",
  turnRight: "Turn right",
  releaseNeck: "Release neck",

  antiFallTitle: "Anti-fall sensor",
  state: "State",
  distance: "Distance",
  lastCommandFrom: "Last command from",
  enabled: "Enabled",
  thresholdMm: "Threshold (mm)",
  save: "Save",
  testNow: "Test now",
  safe: "SAFE",
  alert: "ALERT",
  dash: "—",

  ledsTitle: "LEDs",
  followState: "Follow state",
  bothOn: "Both on",
  bothOff: "Both off",
  leftOnly: "Left only",
  rightOnly: "Right only",

  advancedTitle: "⚙ Hardware / Advanced",
  motorBackendTitle: "Motor backend",
  type: "Type",
  continuousServo: "Continuous servo",
  l298n: "L298N DC motor",
  applyReboots: "Apply (reboots)",
  l298nPins: "L298N pins",
  servoCalibration: "Servo calibration (live-apply)",
  stopPulseUs: "Stop pulse (us)",
  set: "Set",
  reverseLeft: "Reverse left",
  reverseRight: "Reverse right",
  refreshConfig: "Refresh config",
  displayTitle: "Display",
  theme: "Theme",
  themeLight: "Light",
  themeDark: "Dark",
  themeOcean: "Ocean",
  apply: "Apply",
  orientation: "Orientation",
  orientationPortrait: "Portrait (128x160)",
  orientationLandscape: "Landscape",

  flashLink: "Flash firmware",
  flashTitle: "Flash firmware",
  flashIntro: "Write a merged firmware image (build/merged-binary.bin) to the board over USB, directly from this browser.",
  flashUnsupported: "This browser does not support the Web Serial API. Use Google Chrome or Microsoft Edge over HTTPS or localhost.",
  flashSelectPort: "Select device",
  flashConnectedTo: "Connected: {chip}",
  flashConnectFailed: "Connection failed",
  flashChooseFile: "Choose firmware file (.bin)",
  flashNoFile: "No file selected",
  flashFileLoaded: "Loaded {name} ({size} bytes)",
  flashButton: "Flash",
  flashInProgress: "Flashing…",
  flashDone: "Flash complete. Device is rebooting.",
  flashFailed: "Flash failed",
  flashOffsetNote: "Assumes a single merged image built with idf.py merge-bin, written at offset 0x0. Do not select a bare app.bin, bootloader.bin, or partition-table.bin here.",
  flashBackLink: "← Back to controller",
};

const vi: Dict = {
  deviceUrlPlaceholder: "ws://192.168.1.50:8080/ws",
  tokenPlaceholder: "Token ghép nối",
  connect: "Kết nối",
  disconnect: "Ngắt kết nối",
  statusConnected: "● Đã kết nối",
  statusDisconnected: "● Chưa kết nối",
  antiFallDash: "Chống rơi —",
  antiFallOk: "Chống rơi OK",
  antiFallAlert: "Chống rơi CẢNH BÁO",
  motorsDash: "Động cơ —",
  motorsIdle: "Động cơ: đứng yên",
  motorsAction: "Động cơ: {action}",

  movementTitle: "Di chuyển",
  safetyPulse: "Xung an toàn (ms)",
  speed: "Tốc độ",
  movementHelp: "Giữ nút để di chuyển, thả ra để dừng. Bàn phím: WASD hoặc phím mũi tên.",
  stopped: "Đã dừng",
  driving: "Đang chạy {direction}",
  connectFirst: "Kết nối trước khi điều khiển",
  commandFailed: "Lệnh thất bại",
  noGamepad: "Không có gamepad",
  gamepadConnected: "Gamepad: {id}",
  directionForward: "tiến",
  directionBackward: "lùi",
  directionLeft: "trái",
  directionRight: "phải",

  armNeckTitle: "Tay & Cổ",
  arm: "Tay",
  neck: "Cổ",
  wave: "Vẫy tay",
  releaseArm: "Thả tay",
  turnLeft: "Quay trái",
  center: "Giữa",
  turnRight: "Quay phải",
  releaseNeck: "Thả cổ",

  antiFallTitle: "Cảm biến chống rơi",
  state: "Trạng thái",
  distance: "Khoảng cách",
  lastCommandFrom: "Lệnh gần nhất từ",
  enabled: "Bật",
  thresholdMm: "Ngưỡng (mm)",
  save: "Lưu",
  testNow: "Đo thử",
  safe: "AN TOÀN",
  alert: "CẢNH BÁO",
  dash: "—",

  ledsTitle: "Đèn LED",
  followState: "Theo trạng thái",
  bothOn: "Bật cả 2",
  bothOff: "Tắt cả 2",
  leftOnly: "Chỉ trái",
  rightOnly: "Chỉ phải",

  advancedTitle: "⚙ Phần cứng / Nâng cao",
  motorBackendTitle: "Driver động cơ",
  type: "Loại",
  continuousServo: "Servo xoay liên tục",
  l298n: "L298N (motor DC)",
  applyReboots: "Áp dụng (khởi động lại)",
  l298nPins: "Chân L298N",
  servoCalibration: "Hiệu chỉnh servo (áp dụng ngay)",
  stopPulseUs: "Xung dừng (us)",
  set: "Đặt",
  reverseLeft: "Đảo chiều trái",
  reverseRight: "Đảo chiều phải",
  refreshConfig: "Làm mới cấu hình",
  displayTitle: "Màn hình",
  theme: "Theme",
  themeLight: "Sáng",
  themeDark: "Tối",
  themeOcean: "Đại dương",
  apply: "Áp dụng",
  orientation: "Hướng màn hình",
  orientationPortrait: "Dọc (128x160)",
  orientationLandscape: "Ngang",

  flashLink: "Nạp firmware",
  flashTitle: "Nạp firmware",
  flashIntro: "Ghi file firmware đã gộp (build/merged-binary.bin) vào board qua USB, trực tiếp từ trình duyệt.",
  flashUnsupported: "Trình duyệt này không hỗ trợ Web Serial API. Dùng Google Chrome hoặc Microsoft Edge, qua HTTPS hoặc localhost.",
  flashSelectPort: "Chọn thiết bị",
  flashConnectedTo: "Đã kết nối: {chip}",
  flashConnectFailed: "Kết nối thất bại",
  flashChooseFile: "Chọn file firmware (.bin)",
  flashNoFile: "Chưa chọn file",
  flashFileLoaded: "Đã nạp {name} ({size} byte)",
  flashButton: "Nạp",
  flashInProgress: "Đang nạp…",
  flashDone: "Nạp xong. Thiết bị đang khởi động lại.",
  flashFailed: "Nạp thất bại",
  flashOffsetNote: "Giả định đây là file đã gộp bằng idf.py merge-bin, ghi tại offset 0x0. Không chọn app.bin, bootloader.bin, hoặc partition-table.bin riêng lẻ ở đây.",
  flashBackLink: "← Về trang điều khiển",
};

const ko: Dict = {
  deviceUrlPlaceholder: "ws://192.168.1.50:8080/ws",
  tokenPlaceholder: "페어링 토큰",
  connect: "연결",
  disconnect: "연결 해제",
  statusConnected: "● 연결됨",
  statusDisconnected: "● 연결 안 됨",
  antiFallDash: "낙하 방지 —",
  antiFallOk: "낙하 방지 정상",
  antiFallAlert: "낙하 방지 경고",
  motorsDash: "모터 —",
  motorsIdle: "모터: 정지",
  motorsAction: "모터: {action}",

  movementTitle: "이동",
  safetyPulse: "안전 펄스 (ms)",
  speed: "속도",
  movementHelp: "방향을 누르고 있으면 이동, 떼면 정지합니다. 키보드: WASD 또는 방향키.",
  stopped: "정지됨",
  driving: "이동 중: {direction}",
  connectFirst: "먼저 연결하세요",
  commandFailed: "명령 실패",
  noGamepad: "게임패드 없음",
  gamepadConnected: "게임패드: {id}",
  directionForward: "전진",
  directionBackward: "후진",
  directionLeft: "좌회전",
  directionRight: "우회전",

  armNeckTitle: "팔 & 목",
  arm: "팔",
  neck: "목",
  wave: "손 흔들기",
  releaseArm: "팔 풀기",
  turnLeft: "왼쪽으로",
  center: "가운데",
  turnRight: "오른쪽으로",
  releaseNeck: "목 풀기",

  antiFallTitle: "낙하 방지 센서",
  state: "상태",
  distance: "거리",
  lastCommandFrom: "최근 명령 출처",
  enabled: "활성화",
  thresholdMm: "임계값 (mm)",
  save: "저장",
  testNow: "지금 측정",
  safe: "안전",
  alert: "경고",
  dash: "—",

  ledsTitle: "LED",
  followState: "상태 따라가기",
  bothOn: "모두 켜기",
  bothOff: "모두 끄기",
  leftOnly: "왼쪽만",
  rightOnly: "오른쪽만",

  advancedTitle: "⚙ 하드웨어 / 고급",
  motorBackendTitle: "모터 드라이버",
  type: "종류",
  continuousServo: "연속 회전 서보",
  l298n: "L298N DC 모터",
  applyReboots: "적용 (재부팅)",
  l298nPins: "L298N 핀",
  servoCalibration: "서보 보정 (즉시 적용)",
  stopPulseUs: "정지 펄스 (us)",
  set: "설정",
  reverseLeft: "왼쪽 반전",
  reverseRight: "오른쪽 반전",
  refreshConfig: "설정 새로고침",
  displayTitle: "디스플레이",
  theme: "테마",
  themeLight: "라이트",
  themeDark: "다크",
  themeOcean: "오션",
  apply: "적용",
  orientation: "화면 방향",
  orientationPortrait: "세로 (128x160)",
  orientationLandscape: "가로",

  flashLink: "펌웨어 플래시",
  flashTitle: "펌웨어 플래시",
  flashIntro: "병합된 펌웨어 이미지(build/merged-binary.bin)를 USB를 통해 브라우저에서 바로 보드에 씁니다.",
  flashUnsupported: "이 브라우저는 Web Serial API를 지원하지 않습니다. HTTPS 또는 localhost에서 Google Chrome이나 Microsoft Edge를 사용하세요.",
  flashSelectPort: "장치 선택",
  flashConnectedTo: "연결됨: {chip}",
  flashConnectFailed: "연결 실패",
  flashChooseFile: "펌웨어 파일 선택 (.bin)",
  flashNoFile: "선택된 파일 없음",
  flashFileLoaded: "{name} 로드됨 ({size} 바이트)",
  flashButton: "플래시",
  flashInProgress: "플래시 중…",
  flashDone: "플래시 완료. 장치가 재부팅 중입니다.",
  flashFailed: "플래시 실패",
  flashOffsetNote: "idf.py merge-bin으로 병합한 단일 이미지를 오프셋 0x0에 쓴다고 가정합니다. app.bin, bootloader.bin, partition-table.bin을 개별적으로 선택하지 마세요.",
  flashBackLink: "← 컨트롤러로 돌아가기",
};

const dictionaries: Record<Lang, Dict> = { en, vi, ko };

function detectInitialLang(): Lang {
  const stored = localStorage.getItem(STORAGE_KEY);
  if (stored === "en" || stored === "vi" || stored === "ko") return stored;
  const browserLang = navigator.language.slice(0, 2);
  if (browserLang === "vi" || browserLang === "ko") return browserLang;
  return "en";
}

let currentLang: Lang = detectInitialLang();

export function getLang(): Lang {
  return currentLang;
}

export function t(key: string, vars?: Record<string, string>): string {
  let str = dictionaries[currentLang][key] ?? dictionaries.en[key] ?? key;
  if (vars) {
    for (const [k, v] of Object.entries(vars)) {
      str = str.replace(`{${k}}`, v);
    }
  }
  return str;
}

export function setLang(lang: Lang) {
  currentLang = lang;
  localStorage.setItem(STORAGE_KEY, lang);
  applyStaticTranslations();
}

export function applyStaticTranslations() {
  document.documentElement.lang = currentLang;

  document.querySelectorAll<HTMLElement>("[data-i18n]").forEach((el) => {
    const key = el.dataset.i18n;
    if (key) el.textContent = t(key);
  });
  document.querySelectorAll<HTMLInputElement>("[data-i18n-placeholder]").forEach((el) => {
    const key = el.dataset.i18nPlaceholder;
    if (key) el.placeholder = t(key);
  });
  document.querySelectorAll<HTMLElement>("[data-i18n-aria]").forEach((el) => {
    const key = el.dataset.i18nAria;
    if (key) el.setAttribute("aria-label", t(key));
  });
}
