> **Câu chuyện nguồn gốc đầy đủ (lưu trữ).** Đây là bản chi tiết đầy đủ về
> nguồn gốc Yana Robot và quan hệ với XiaoZhi AI Chatbot (MIT). File
> [`README.md`](../README_vi.md) ở gốc repo vẫn giữ một đoạn ghi công ngắn
> gọn tương đương — không có gì giấu ở đây cả, chỉ là chuyển ra khỏi trang
> chủ để trang đó có thể nói về Yana Wheelbot làm được gì trước.
> Xem thêm: [English](origin-story.md) · [한국어](origin-story_ko.md).

---

# Yana Robot

(Tiếng Việt | [English](origin-story.md) | [한국어](origin-story_ko.md))

Nền tảng robot voice-AI ESP32-S3 độc lập, phát triển từ
[XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32) (MIT) — điều khiển
động cơ/servo, an toàn chống rơi, và điều khiển qua giọng nói + web dựa trên MCP.

<img src="mcp-based-graph.jpg" alt="Điều khiển mọi thứ qua MCP" width="320">

## Hiện tại thực sự là gì

Dự án này bắt đầu từ việc clone XiaoZhi AI Chatbot — một firmware trợ lý
giọng nói ESP32 mã nguồn mở thật sự (MIT). Phần nền tảng kế thừa — truyền
tải WebSocket và MQTT+UDP, phát hiện wake-word ngoại tuyến, pipeline
ASR/LLM/TTS dạng streaming, điều khiển MCP cả ở thiết bị lẫn cloud — vẫn
hoạt động, trên khoảng 138 thư mục board mà XiaoZhi hỗ trợ (xem
[supported-boards.md](supported-boards.md)).

Phần đang được xây dựng chủ động dưới tên Yana Robot là
**[`yana-wheelbot`](../main/boards/yana-wheelbot)** — một board robot 2 bánh
làm từ đầu, cùng **[`apps/controller-web`](../apps/controller-web)**, bảng
điều khiển web cho nó. Mọi thứ board này làm — di chuyển, cảm biến chống
rơi, LED, servo tay/cổ, màn hình — đều được expose thành MCP tool, điều
khiển được cục bộ qua WebSocket hoặc bởi một backend voice AI nói cùng giao
thức (ví dụ nền tảng [Yana AI](https://github.com/yanacuti1121/Yana-AI) của
tác giả, qua `tools/yana-web/robot.js`).

**Trạng thái thật:** `yana-wheelbot` mới chỉ build-verified — compile sạch,
chân GPIO mặc định đã đối chiếu với một thiết kế tham khảo công khai thật,
nhưng **chưa có robot thật nào được lắp và chạy thử**. Xem README riêng của
board để biết danh sách cụ thể những gì còn cần kiểm chứng trên phần cứng
trước khi coi là ổn.

## Yana Wheelbot

<img src="../main/boards/yana-wheelbot/wiring-diagram.svg" alt="Sơ đồ đấu nối Yana Wheelbot" width="480">

- Di chuyển differential-drive, driver động cơ chọn được lúc chạy (servo
  xoay liên tục hoặc L298N DC)
- Cảm biến chống rơi VL53L0X (VL6180X là tùy chọn lúc build)
- 2 LED, servo tay + cổ, cảm biến chạm TTP223
- Đổi được hướng/theme màn hình (mặc định ST7789, ST7735 là tùy chọn build)
- Danh sách linh kiện đầy đủ, sơ đồ đấu nối, và tham khảo MCP tool:
  [main/boards/yana-wheelbot](../main/boards/yana-wheelbot) ·
  [English](../main/boards/yana-wheelbot/README.md) ·
  [한국어](../main/boards/yana-wheelbot/README_ko.md)
- Bảng điều khiển web: [apps/controller-web](../apps/controller-web)

## Nguồn gốc

Yana Robot được xây dựng dựa trên nền tảng của [XiaoZhi AI Chatbot](https://github.com/78/xiaozhi-esp32) — dự án firmware trợ lý giọng nói ESP32 mã nguồn mở gốc, do Xiaoqiang ([78](https://github.com/78)) và Shenzhen Xinzhi Future Technology Co., Ltd. tạo ra. Toàn bộ công lao cho codebase gốc, thiết kế giao thức và hệ sinh thái phần cứng thuộc về dự án đó và các contributor của họ.

Repo này là một nhánh phát triển độc lập: không liên kết và không đồng bộ với dự án gốc. Từ thời điểm này trở đi, board mới, bản sửa lỗi và tính năng trong repo này được thiết kế và duy trì riêng dưới tên Yana Robot.

## Yêu cầu

- ESP-IDF v6.0 trở lên; v6.0.2 là SDK ổn định được khuyến nghị. v5.5.2 chỉ
  giữ lại cho các board cũ đã tài liệu hóa (chủ yếu liên quan bộ board kế
  thừa, không phải `yana-wheelbot`). Xem
  [ESP-IDF 6.0 Migration Guide](esp-idf-6-migration.md) để biết chi
  tiết tương thích và trạng thái xác nhận từng board.
- Cursor hoặc VS Code với plugin ESP-IDF. Linux build nhanh hơn và ít lỗi
  driver hơn Windows.
- Dự án này theo chuẩn code style C++ của Google.

## Tài liệu

- [Board Yana Wheelbot](../main/boards/yana-wheelbot) — linh kiện, đấu nối, MCP tool
- [Bảng điều khiển web Wheelbot](../apps/controller-web) — control panel trên trình duyệt
- [Custom Board Guide](custom-board.md) — hướng dẫn tạo board mới
- [MCP Protocol IoT Control Usage](mcp-usage.md)
- [MCP Protocol Interaction Flow](mcp-protocol.md)
- [Giao thức WebSocket](websocket.md) · [Giao thức MQTT + UDP](mqtt-udp.md)
- [Phần cứng được hỗ trợ](supported-boards.md) — danh sách board kế thừa đầy đủ, danh sách tính năng, và hệ sinh thái bên thứ ba tương thích
- [ESP-IDF 6.0 Migration Guide](esp-idf-6-migration.md)

Firmware kết nối tới server chính thức [xiaozhi.me](https://xiaozhi.me) theo
mặc định nếu anh không trỏ nó sang backend riêng — người dùng cá nhân có
thể đăng ký tài khoản ở đó miễn phí.

## Về dự án này

Yana Robot được phát hành theo giấy phép MIT, cho phép bất kỳ ai sử dụng miễn phí, kể cả cho mục đích thương mại. File license giữ nguyên thông báo bản quyền gốc từ dự án XiaoZhi thượng nguồn, theo đúng yêu cầu của giấy phép đó.

Firmware gốc, thiết kế giao thức, và hệ sinh thái board được tạo ra bởi [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32) và các contributor của họ. Mọi thứ trong repo này từ thời điểm này trở đi — board mới, sửa lỗi, và tính năng mới — được thiết kế và duy trì độc lập dưới tên Yana Robot, không còn liên kết với dự án thượng nguồn.

Nếu anh có ý tưởng hay đề xuất gì, hãy mở Issue trên repo này.

## Star History

<a href="https://star-history.com/#yanacuti1121/yana-wheelbot&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
   <img alt="Star History Chart" src="https://api.star-history.com/svg?repos=yanacuti1121/yana-wheelbot&type=Date" />
 </picture>
</a>
