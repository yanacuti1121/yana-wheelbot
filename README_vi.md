# Yana Robot

(Tiếng Việt | [English](README.md) | [한국어](README_ko.md))

Nền tảng robot voice-AI ESP32-S3: điều khiển động cơ/servo, an toàn chống
rơi, và điều khiển qua giọng nói + web dựa trên MCP, chạy trên bản fork
riêng của Yana từ nền firmware XiaoZhi AI Chatbot.

## Yana Wheelbot

<img src="main/boards/yana-wheelbot/wiring-diagram.svg" alt="Sơ đồ đấu nối Yana Wheelbot" width="480">

- Di chuyển differential-drive, driver động cơ chọn được lúc chạy (servo
  xoay liên tục hoặc L298N DC)
- Cảm biến chống rơi VL53L0X (VL6180X là tùy chọn lúc build), có cơ chế
  khóa chặn di chuyển khi cảm biến báo không an toàn, không chỉ dừng lần
  di chuyển hiện tại
- 2 LED, servo tay + cổ, cảm biến chạm TTP223
- Đổi được hướng/theme màn hình (mặc định ST7789, ST7735 là tùy chọn build)
- Danh sách linh kiện đầy đủ, sơ đồ đấu nối, và tham khảo MCP tool:
  [main/boards/yana-wheelbot](main/boards/yana-wheelbot) ·
  [English](main/boards/yana-wheelbot/README.md) ·
  [한국어](main/boards/yana-wheelbot/README_ko.md)
- Bảng điều khiển web, đa ngôn ngữ EN/VI/KO: [apps/controller-web](apps/controller-web)
- Chassis in 3D (BOM, đấu dây, hướng dẫn lắp ráp):
  [Tiếng Việt](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_vi.md) ·
  [English](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE.md) ·
  [한국어](main/boards/yana-wheelbot/chassis/CHASSIS_GUIDE_ko.md)

**Trạng thái thật:** `yana-wheelbot` mới chỉ build-verified — compile
sạch, chân GPIO mặc định đã đối chiếu với một thiết kế tham khảo công khai
thật, nhưng chưa có robot thật nào được lắp và chạy thử. Xem README riêng
của board để biết danh sách cụ thể những gì còn cần kiểm chứng trên phần
cứng trước khi coi là ổn.

## Yêu cầu

- ESP-IDF v6.0 trở lên; v6.0.2 là SDK ổn định được khuyến nghị. Xem
  [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md) để biết chi
  tiết tương thích.
- Cursor hoặc VS Code với plugin ESP-IDF. Linux build nhanh hơn và ít lỗi
  driver hơn Windows.
- Dự án này theo chuẩn code style C++ của Google.

## Tài liệu

- [Board Yana Wheelbot](main/boards/yana-wheelbot) — linh kiện, đấu nối, MCP tool
- [Bảng điều khiển web Wheelbot](apps/controller-web) — control panel trên trình duyệt
- [Custom Board Guide](docs/custom-board.md) — hướng dẫn tạo board mới
- [MCP Protocol IoT Control Usage](docs/mcp-usage.md)
- [MCP Protocol Interaction Flow](docs/mcp-protocol.md)
- [Giao thức WebSocket](docs/websocket.md) · [Giao thức MQTT + UDP](docs/mqtt-udp.md)
- [Phần cứng được hỗ trợ](docs/supported-boards.md)
- [ESP-IDF 6.0 Migration Guide](docs/esp-idf-6-migration.md)

Firmware kết nối tới server chính thức [xiaozhi.me](https://xiaozhi.me) theo
mặc định nếu anh không trỏ nó sang backend riêng — người dùng cá nhân có
thể đăng ký tài khoản ở đó miễn phí.

## Giấy phép và nguồn gốc

Yana Robot phát hành theo giấy phép MIT — dùng miễn phí, kể cả cho mục
đích thương mại. Dự án được xây dựng trên nền
[XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32) (cũng MIT), do
Xiaoqiang ([78](https://github.com/78)) và Shenzhen Xinzhi Future
Technology Co., Ltd. tạo ra; file LICENSE giữ nguyên thông báo bản quyền
gốc của họ theo đúng yêu cầu giấy phép. Repo này là một nhánh phát triển
độc lập, không liên kết hay đồng bộ với dự án gốc — xem
[docs/origin-story_vi.md](docs/origin-story_vi.md) để đọc đầy đủ phần nào
kế thừa, phần nào tự xây mới.

Nếu anh có ý tưởng hay đề xuất gì, hãy mở Issue trên repo này.
