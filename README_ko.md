# Yana Robot

([English](README.md) | [Tiếng Việt](README_vi.md) | 한국어)

ESP32-S3 음성 AI 로보틱스 플랫폼 — 모터/서보 제어, 낙하 방지 안전, MCP 기반
음성 + 웹 제어를 제공하며, Yana가 XiaoZhi AI Chatbot 펌웨어를 기반으로 독립
포크한 위에서 동작합니다.

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/chassis/chassis-render.png" alt="Yana Wheelbot 섀시, OpenSCAD 렌더" width="480">
<img src="main/boards/yana-wheelbot/wiring-diagram.svg" alt="Yana Wheelbot 배선도" width="480">

- 차동 구동 이동, 런타임에 선택 가능한 모터 드라이버(연속 회전 서보 또는
  L298N DC 드라이버)
- VL53L0X 낙하 방지 센서 (VL6180X는 빌드 옵션으로 지원), 센서가 위험을
  감지하면 다음 이동 명령까지 막는 잠금(latch) 방식 적용
- 듀얼 LED, 팔 + 목 서보, TTP223 터치 센서
- 전환 가능한 디스플레이 방향/테마 (기본값 ST7789, ST7735은 빌드 옵션)
- 전체 부품 목록, 배선도, MCP 툴 레퍼런스:
  [main/boards/yana-wheelbot](main/boards/yana-wheelbot) ·
  [English](main/boards/yana-wheelbot/README.md) ·
  [Tiếng Việt](main/boards/yana-wheelbot/README_vi.md)
- 웹 제어 패널 (한/영/베 다국어 지원): [apps/controller-web](apps/controller-web)
- 3D 프린트 섀시 (BOM, 배선, 조립 가이드):
  [한국어](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_ko.md) ·
  [English](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE.md) ·
  [Tiếng Việt](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_vi.md)

**솔직한 현재 상태:** `yana-wheelbot`은 빌드 검증만 되어 있습니다 — 깔끔하게
컴파일되고 기본 GPIO 값은 실제 공개된 참조 설계와 대조 확인되었지만,
**아직 실제 물리적 로봇이 조립되어 동작한 적이 없습니다**. 완전히
신뢰하기 전에 실제 하드웨어에서 검증이 필요한 구체적인 항목 목록은 보드
자체 README를 참고하세요.

## 휴대폰 또는 컴퓨터에서 제어하기

Yana Wheelbot은 현재 `/robot` 웹 페이지를 직접 제공하지 않습니다.
[`apps/controller-web`](apps/controller-web)의 제어기를 Mac, Windows 또는
Linux 컴퓨터에서 실행하고 같은 로컬 Wi-Fi의 로봇에 직접 연결합니다.

### 연결 전 준비

- `yana-wheelbot` 펌웨어를 플래시하고 로봇을 2.4 GHz Wi-Fi에 연결합니다.
- 제어기 컴퓨터와 휴대폰을 같은 LAN에 연결합니다. 게스트 Wi-Fi와
  AP/client isolation은 사용할 수 없습니다.
- 라우터의 DHCP/클라이언트 목록이나 ESP-IDF 시리얼 로그에서 로봇 IP를
  확인합니다.
- 인증된 음성/클라우드 세션에서 `self.local_control.get_token` 툴을 사용해
  6자리 로컬 제어 토큰을 가져옵니다.
- 웹 UI를 실행할 컴퓨터에 Node.js 18 이상과 npm을 설치합니다.

### 제어기 시작

```sh
cd apps/controller-web
npm install                 # 최초 한 번만
npm run dev:lan
```

Vite가 **Local** 주소와 **Network** 주소를 출력합니다.

| 접속 장치 | 열 주소 |
|---|---|
| 제어기를 실행 중인 컴퓨터 | `http://localhost:5173` |
| iPhone, Android 또는 다른 컴퓨터 | 출력된 Network URL, 예: `http://192.168.1.20:5173` |

휴대폰의 `localhost`는 제어기를 실행하는 컴퓨터가 아니라 휴대폰 자체를
의미합니다. macOS 또는 Windows가 요청하면 로컬 네트워크/방화벽 접근을
허용하세요.

Yana Wheelbot 페이지에 다음 값을 입력합니다.

- **Device URL:** `ws://<로봇-IP>:8080/ws`
- **Pairing token:** 위에서 얻은 6자리 토큰

페이지가 WebSocket 핸드셰이크 URL에 `?token=<코드>`를 자동으로 추가합니다.
**Connect**를 누른 뒤 첫 테스트에서는 바퀴를 바닥에서 들어 올리고 **STOP**을
확인한 다음 바닥 주행을 시험하세요. 이동 제어는 짧은 안전 펄스를 계속
갱신하며, 버튼을 놓거나 브라우저가 포커스를 잃거나 연결이 끊어지면 모터가
정지합니다.

### 연결 문제 해결

- **Network URL이 열리지 않음:** `npm run dev:lan`을 사용하고 호스트
  방화벽과 동일한 비게스트 Wi-Fi 연결을 확인하세요.
- **페이지는 열리지만 로봇 연결 실패:** 로봇 IP, 포트 `8080`, 토큰 및
  AP/client isolation 설정을 확인하세요.
- **HTTP 401 또는 즉시 WebSocket 종료:** 토큰이 없거나 잘못되었습니다.
  토큰을 다시 가져오고, 노출이 의심되면
  `self.local_control.rotate_token`으로 교체하세요.
- **HTTPS 페이지에서 `ws://` 연결 실패:** 브라우저의 mixed-content
  차단입니다. 위의 로컬 HTTP 실행 방법을 사용하세요. 현재 펌웨어는
  `wss://`를 제공하지 않습니다.
- 포트 `8080`을 인터넷으로 포트 포워딩하지 마세요. 로컬 제어에는 토큰이
  있지만 암호화되지 않으므로 신뢰할 수 있는 LAN에서만 사용해야 합니다.

### 프로토콜 호환 상태

소스 수준 점검 결과 웹 UI가 사용하는 25개 MCP 툴 이름과 인자 형식이 모두
펌웨어 등록과 일치합니다. 핸드셰이크 URL은
`ws://<로봇-IP>:8080/ws?token=<코드>`이며 text frame은 JSON-RPC 2.0과 MCP
버전 `2024-11-05`를 사용합니다. 구현된 method는 `initialize`, `tools/list`,
`tools/call`입니다.

이는 펌웨어의 MCP 호환 JSON-RPC 하위 집합이며 범용 MCP transport 전체
구현은 아닙니다. 응답은 현재 인증된 모든 로컬 클라이언트에 전달되므로
request ID 충돌을 피하려면 로컬 제어기는 한 번에 하나만 사용하세요. 음성
명령과 로컬 명령도 겹칠 수 있으며 가장 최근의 이동 명령이 우선합니다.
이 경로는 소스/빌드 수준에서 검증되었지만 실제 로봇에서 end-to-end 테스트가
필요합니다.

## 요구 사항

- ESP-IDF v6.0 이상; v6.0.2가 권장 안정 SDK입니다. 전체 호환성은
  [ESP-IDF 6.0 마이그레이션 가이드](docs/esp-idf-6-migration.md)를 참고하세요.
- ESP-IDF 플러그인이 설치된 Cursor 또는 VS Code. Windows보다 Linux가
  컴파일이 빠르고 드라이버 문제가 적습니다.
- 이 프로젝트는 Google C++ 코드 스타일을 따릅니다.

## 문서

- [Yana Wheelbot 보드](main/boards/yana-wheelbot) — 부품, 배선, MCP 툴
- [Wheelbot 웹 제어기](apps/controller-web) — 브라우저 제어 패널
- [Custom Board Guide](docs/custom-board.md) — 새 보드 만들기
- [MCP Protocol IoT Control Usage](docs/mcp-usage.md)
- [MCP Protocol Interaction Flow](docs/mcp-protocol.md)
- [WebSocket 프로토콜](docs/websocket.md) · [MQTT + UDP 프로토콜](docs/mqtt-udp.md)
- [지원 하드웨어](docs/supported-boards.md)
- [ESP-IDF 6.0 마이그레이션 가이드](docs/esp-idf-6-migration.md)

자체 백엔드로 연결하지 않으면 펌웨어는 기본적으로 공식
[xiaozhi.me](https://xiaozhi.me) 서버에 연결됩니다 — 개인 사용자는 해당
사이트에서 무료로 계정을 등록해 사용할 수 있습니다.

## 라이선스와 출처

Yana Robot은 MIT 라이선스로 배포되어 상업적 용도를 포함해 누구나 무료로
사용할 수 있습니다. [XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32)
(역시 MIT)을 기반으로 만들어졌으며, 원저작자는 Xiaoqiang
([78](https://github.com/78))과 Shenzhen Xinzhi Future Technology
Co., Ltd.입니다. LICENSE 파일은 라이선스 요구사항에 따라 원본 저작권
표시를 그대로 보존하고 있습니다. 이 저장소는 독립적인 후속 프로젝트로,
업스트림 프로젝트와 제휴하거나 이를 추적하지 않습니다 — 무엇이 물려받은
것이고 무엇이 새로 만든 것인지 전체 내용은
[docs/origin-story_ko.md](docs/origin-story_ko.md)를 참고하세요.

아이디어나 제안이 있다면 이 저장소에 Issue를 열어주세요.
