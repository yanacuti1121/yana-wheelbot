# Yana Robot

([English](README.md) | [Tiếng Việt](README_vi.md) | 한국어)

[XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32)(MIT)에서 파생된
독립적인 ESP32-S3 음성 AI 로보틱스 플랫폼 — 모터/서보 제어, 낙하 방지 안전,
MCP 기반 음성 + 웹 제어를 제공합니다.

<img src="docs/mcp-based-graph.jpg" alt="MCP로 모든 것을 제어" width="320">

## 지금 실제로 무엇인가

이 프로젝트는 진짜 오픈소스(MIT)인 ESP32 음성 어시스턴트 펌웨어 XiaoZhi AI
Chatbot을 클론하면서 시작되었습니다. 물려받은 플랫폼 부분 — WebSocket 및
MQTT+UDP 전송, 오프라인 웨이크 워드 감지, 스트리밍 ASR/LLM/TTS 파이프라인,
디바이스 측 + 클라우드 측 MCP 툴 제어 — 는 XiaoZhi가 지원하는 약 138개
보드 디렉토리에서 여전히 동작합니다 ([docs/supported-boards.md](docs/supported-boards.md) 참고).

Yana Robot이라는 이름 아래 실제로 능동적으로 개발되고 있는 부분은
처음부터 새로 만든 2륜 로봇 보드인
**[`yana-wheelbot`](main/boards/yana-wheelbot)**과, 그 보드용 웹 제어
패널인 **[`apps/controller-web`](apps/controller-web)**입니다. 이 보드가
하는 모든 것 — 이동, 낙하 방지 센싱, LED, 팔/목 서보, 디스플레이 — 은 모두
MCP 툴로 노출되어 있으며, 로컬에서 WebSocket으로 직접 제어하거나 동일한
프로토콜을 구사하는 음성 AI 백엔드(예: 저자 본인의
[Yana AI](https://github.com/yanacuti1121/Yana-AI) 플랫폼, `tools/yana-web/robot.js`
경유)로 제어할 수 있습니다.

**솔직한 현재 상태:** `yana-wheelbot`은 빌드 검증만 되어 있습니다 — 깔끔하게
컴파일되고 기본 GPIO 값은 실제 공개된 참조 설계와 대조 확인되었지만,
**아직 실제 물리적 로봇이 조립되어 동작한 적이 없습니다**. 완전히
신뢰하기 전에 실제 하드웨어에서 검증이 필요한 구체적인 항목 목록은 보드
자체 README를 참고하세요.

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/wiring-diagram.svg" alt="Yana Wheelbot 배선도" width="480">

- 차동 구동 이동, 런타임에 선택 가능한 모터 드라이버(연속 회전 서보 또는
  L298N DC 드라이버)
- VL53L0X 낙하 방지 센서 (VL6180X는 빌드 옵션으로 지원)
- 듀얼 LED, 팔 + 목 서보, TTP223 터치 센서
- 전환 가능한 디스플레이 방향/테마 (기본값 ST7789, ST7735은 빌드 옵션)
- 전체 부품 목록, 배선도, MCP 툴 레퍼런스:
  [main/boards/yana-wheelbot](main/boards/yana-wheelbot) ·
  [English](main/boards/yana-wheelbot/README.md) ·
  [Tiếng Việt](main/boards/yana-wheelbot/README_vi.md)
- 웹 제어 패널: [apps/controller-web](apps/controller-web)

## 출처

Yana Robot은 원본 오픈소스 ESP32 음성 어시스턴트 펌웨어를 만든
Xiaoqiang([78](https://github.com/78))과 Shenzhen Xinzhi Future Technology
Co., Ltd.의 [XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32)
프로젝트를 기반으로 만들어졌습니다. 원본 코드베이스, 프로토콜 설계, 하드웨어
생태계에 대한 모든 공로는 해당 프로젝트와 기여자들에게 있습니다.

이 저장소는 독립적인 후속 프로젝트입니다: 업스트림 프로젝트와 제휴하지
않으며 이를 추적하지도 않습니다. 이 시점 이후로 이 저장소의 보드, 수정
사항, 기능은 Yana Robot이라는 이름으로 독립적으로 설계 및 유지보수됩니다.

## 요구 사항

- ESP-IDF v6.0 이상; v6.0.2가 권장 안정 SDK입니다. v5.5.2는 문서화된
  레거시 보드(대부분 물려받은 보드 세트와 관련, `yana-wheelbot`과는 무관)를
  위해서만 유지됩니다. 전체 호환성 및 보드 검증 상태는
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
- [지원 하드웨어](docs/supported-boards.md) — 물려받은 전체 보드 목록, 기능 목록, 호환 가능한 서드파티 생태계
- [ESP-IDF 6.0 마이그레이션 가이드](docs/esp-idf-6-migration.md)

자체 백엔드로 연결하지 않으면 펌웨어는 기본적으로 공식
[xiaozhi.me](https://xiaozhi.me) 서버에 연결됩니다 — 개인 사용자는 해당
사이트에서 무료로 계정을 등록해 사용할 수 있습니다.

## 이 프로젝트에 대해

Yana Robot은 MIT 라이선스로 배포되어 상업적 용도를 포함해 누구나 무료로
사용할 수 있습니다. 라이선스 파일은 해당 라이선스가 요구하는 대로 업스트림
XiaoZhi 프로젝트의 원본 저작권 표시를 보존하고 있습니다.

원본 펌웨어, 프로토콜 설계, 보드 생태계는 [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)와 그 기여자들이 만들었습니다. 이 시점 이후 이 저장소의 모든 것 — 새 보드, 버그 수정, 기능 작업 — 은 업스트림 프로젝트와 더 이상 관계 없이 Yana Robot이라는 이름으로 독립적으로 설계 및 유지보수됩니다.

아이디어나 제안이 있다면 이 저장소에 Issue를 열어주세요.

## Star History

<a href="https://star-history.com/#yanacuti1121/yana-wheelbot&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
 </picture>
</a>
