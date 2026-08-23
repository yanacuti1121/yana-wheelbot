# Yana Robot

([English](README.md) | [Tiếng Việt](README_vi.md) | 한국어)

ESP32-S3 음성 AI 로보틱스 플랫폼 — 모터/서보 제어, 낙하 방지 안전, MCP 기반
음성 + 웹 제어를 제공하며, Yana가 XiaoZhi AI Chatbot 펌웨어를 기반으로 독립
포크한 위에서 동작합니다.

<img src="docs/mcp-based-graph.jpg" alt="MCP로 모든 것을 제어" width="320">

## Yana Wheelbot

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
  [main/boards/yana-wheelbot/chassis](main/boards/yana-wheelbot/chassis)

**솔직한 현재 상태:** `yana-wheelbot`은 빌드 검증만 되어 있습니다 — 깔끔하게
컴파일되고 기본 GPIO 값은 실제 공개된 참조 설계와 대조 확인되었지만,
**아직 실제 물리적 로봇이 조립되어 동작한 적이 없습니다**. 완전히
신뢰하기 전에 실제 하드웨어에서 검증이 필요한 구체적인 항목 목록은 보드
자체 README를 참고하세요.

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

## Star History

<a href="https://star-history.com/#yanacuti1121/yana-wheelbot&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
 </picture>
</a>
