# Yana Wheelbot

(Tiếng Việt | [English](README.md) | [한국어](README_ko.md))

Board robot 2 bánh (differential-drive) trên nền tảng Yana Robot / XiaoZhi.
Thêm: driver động cơ chọn được (servo liên tục hoặc L298N), cảm biến chống
rơi ToF, 2 LED, servo tay + cổ, và màn hình đổi được hướng/theme — bên trên
phần voice AI + màn hình biểu cảm có sẵn của nền tảng.

## Linh kiện cần có

| Bộ phận | Linh kiện gợi ý | Ghi chú |
|---|---|---|
| Vi điều khiển | ESP32-S3-WROOM-1 N16R8 (16MB flash / 8MB PSRAM) | `target: esp32s3` trong `config.json` |
| Driver động cơ (chọn 1) | 2x servo xoay liên tục 360° | Kéo bánh trực tiếp, không cần driver rời |
| | *hoặc* L298N + 2 động cơ DC | Xem lưu ý về quy ước chân IN1-4 ở phần "Chưa xác minh" |
| Cảm biến chống rơi (ToF, I2C) | VL53L0X (mặc định, tầm ~2m) | |
| | *hoặc* VL6180X / TOF050C (build option, tầm ~200mm) | Xem "Chọn biến thể lúc build" |
| LED | 2x LED đơn (trái/phải) | |
| Servo tay + cổ | 2x servo góc thường (0-180°) | |
| Cảm biến chạm | Module cảm ứng chạm TTP223 | Chạm đôi để bật/tắt trò chuyện, giống nút boot |
| Mic | I2S, kiểu INMP441 hoặc tương đương | |
| Loa | I2S amp kiểu MAX98357A + loa 3W/4Ω | Cần nguồn 5V riêng đủ mạnh — xem lưu ý nguồn bên dưới |
| Màn hình | SPI ST7789 128x160 (mặc định) | |
| | *hoặc* ST7735 (build option) | Xem "Chọn biến thể lúc build" |
| Nút Boot | có sẵn trên board ESP32-S3 (GPIO0) | Không cần thêm |
| Nguồn | Pin Li-ion/LiPo 3.7V, ~2000mAh | |
| | Mạch sạc + tăng áp 5V TP4056 Type-C | |
| | Công tắc nguồn ON/OFF | |

**Lưu ý nguồn:** loa khuếch đại và 2 servo bánh xe cần nguồn 5V riêng đủ
mạnh (không lấy thẳng từ bộ ổn áp 3.3V của MCU) — một mạch tăng áp như
TP4056 lo được việc này từ 1 cell LiPo 3.7V. Tất cả GND (MCU, amp, servo,
cảm biến) vẫn phải nối chung.

## Sơ đồ đấu nối (mặc định)

![Sơ đồ đấu nối Yana Wheelbot](wiring-diagram.svg)

Chân GPIO mặc định lấy theo sơ đồ đấu nối công khai của
[KST AI Robot](https://ai.kenhsangtao.com/) (xem mục "Credit" bên dưới) —
mọi chân dưới đây đều đã xác nhận khớp với sơ đồ đó, trừ backlight màn hình
là tự chọn riêng của dự án này (đánh dấu ở cột Ghi chú). Toàn bộ định nghĩa
nằm ở `config.h`.

| Bộ phận | Tín hiệu | GPIO | Ghi chú |
|---|---|---|---|
| Động cơ (L298N) | IN1 (trái, PWM/EN) | 38 | Đổi được lúc chạy qua `self.wheelbot.set_motor_pins` |
| | IN2 (trái, DIR) | 39 | |
| | IN3 (phải, PWM/EN) | 40 | |
| | IN4 (phải, DIR) | 41 | |
| Servo bánh (nếu dùng servo thay L298N) | Trái | 47 | |
| | Phải | 45 | |
| Cảm biến ToF (I2C) | SDA | 1 | |
| | SCL | 2 | |
| LED | Trái | 3 | |
| | Phải | 18 | |
| Servo tay | Tín hiệu | 20 | |
| Servo cổ | Tín hiệu | 21 | |
| Cảm biến chạm (TTP223) | OUT | 7 | Active-high; chạm đôi để bật/tắt trò chuyện |
| Mic (I2S) | WS | 4 | |
| | SCK | 5 | |
| | DIN | 6 | |
| Loa (I2S) | DIN | 17 | |
| | BCLK | 16 | |
| | LRCK | 15 | |
| Màn hình (SPI) | Backlight | 9 | Tự chọn riêng của dự án này — module của KST nối BL thẳng vào 3V3, không điều khiển bằng phần mềm |
| | MOSI | 11 | |
| | CLK | 12 | |
| | DC | 10 | |
| | RST | 14 | |
| | CS | 13 | |

Chân IN1-4 của motor (và loại motor servo/L298N) có thể đổi lúc chạy qua MCP
tool, lưu vào NVS, không cần nạp lại firmware — các giá trị trên chỉ là mặc
định lúc mới nạp.

**Không dùng GPIO36/GPIO37** — dành riêng cho PSRAM trên module
ESP32-S3-WROOM-1 N16R8.

## Chọn biến thể lúc build

Mặc định build cho ST7789 + VL53L0X. Nếu linh kiện thực tế là ST7735 và/hoặc
VL6180X, bật 2 tùy chọn Kconfig sau trước khi build (`idf.py menuconfig` →
"Yana Wheelbot" hoặc sửa thẳng `sdkconfig`):

```
CONFIG_YANA_WHEELBOT_DISPLAY_ST7735=y   # dùng driver ST7735 thay vì ST7789
CONFIG_YANA_WHEELBOT_TOF_VL6180X=y      # dùng cảm biến VL6180X thay vì VL53L0X
```

Sau khi đổi, chạy `idf.py fullclean` rồi build lại.

## Giao thức điều khiển

Board này nói 2 giao thức, dùng chung một bộ MCP tool:

**1. Điều khiển cục bộ (LAN, không qua cloud)** — giống hệt cách
`main/boards/otto-robot` làm: board mở WebSocket server ở cổng 8080, nhận
thẳng message theo đúng khuôn JSON-RPC 2.0 mà MCP dùng, không cần đi qua
backend AI. `apps/controller-web` (trong repo này) là một client web sẵn
dùng cho giao thức này.

**Cần token ghép nối** — mã ngẫu nhiên 6 số, tạo 1 lần lúc mới boot và lưu
NVS, bắt buộc phải kèm dạng `?token=<mã>` trên URL handshake. Nếu không có
cơ chế này, bất kỳ ai cùng mạng Wi-Fi cũng điều khiển được robot hoặc đổi
GPIO động cơ mà không cần xác thực gì. Lấy token qua MCP tool
`self.local_control.get_token` từ một kênh đã xác thực sẵn (ví dụ hỏi qua
giọng nói/cloud), hoặc đổi mới bằng `self.local_control.rotate_token`.

**Kết nối:** `ws://<ip-của-board>:8080/ws?token=<mã>`

```json
{"jsonrpc":"2.0","method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{}},"id":1}
{"jsonrpc":"2.0","method":"tools/list","params":{},"id":2}
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.wheelbot.move_forward","arguments":{"duration_ms":2000,"speed":80}},"id":3}
```

**2. Điều khiển bằng giọng nói (cloud/backend AI)** — board đóng vai trò MCP
**server** (giữ và thực thi tool), backend AI đóng vai trò MCP **client** —
đúng giao thức WebSocket chuẩn của XiaoZhi/yana-robot
(`docs/websocket.md`, `docs/mcp-protocol.md` ở gốc repo). Không cần sửa
firmware để đổi backend — chỉ cần trỏ server URL của board sang bất kỳ
backend nào nói đúng giao thức này (ví dụ dự án `Yana-AI`'s
`tools/yana-web/robot.js`).

### Danh sách tool

| Tool | Tham số | Ghi chú |
|---|---|---|
| `self.wheelbot.move_forward` | `duration_ms` (0-30000, mặc định 2000), `speed` (0-100, mặc định 80) | lệnh mới thắng ngay cả khi 1 lệnh khác đang chạy dở (ngắt luôn, không chỉ xóa lệnh đang chờ) |
| `self.wheelbot.move_backward` | như trên | |
| `self.wheelbot.turn_left` | như trên | quay tại chỗ |
| `self.wheelbot.turn_right` | như trên | quay tại chỗ |
| `self.wheelbot.stop` | — | dừng khẩn cấp, ngắt lệnh đang chạy |
| `self.wheelbot.set_motor_type` | `type`: `"servo"` \| `"l298n"` | lưu NVS, khởi động lại để áp dụng; string khác bị từ chối kèm lỗi, không tự hiểu ngầm thành giá trị khác |
| `self.wheelbot.set_motor_pins` | `in1`,`in2`,`in3`,`in4` (số GPIO) | chỉ áp dụng cho L298N; lưu NVS, khởi động lại; từ chối pin trùng nhau hoặc pin dành riêng (flash/PSRAM, không tồn tại, hoặc strapping pin) trước khi lưu |
| `self.wheelbot.get_motor_config` | — | trả về backend + chân + hiệu chỉnh hiện tại (JSON) |
| `self.wheelbot.get_control_status` | — | có đang di chuyển không, và kênh nào (`local_ws` hay `cloud_voice`) gửi lệnh di chuyển gần nhất — xem "Nguồn lệnh" bên dưới |
| `self.wheelbot.set_servo_stop_pulse` | `microseconds` (1000-2000, mặc định 1500) | áp dụng ngay; chỉ backend servo |
| `self.wheelbot.set_servo_reverse` | `side`: `"left"`\|`"right"`, `reversed` (bool) | áp dụng ngay; chỉ backend servo; giá trị `side` khác bị từ chối kèm lỗi |
| `self.cliff_sensor.set_enabled` | `enabled` (bool) | |
| `self.cliff_sensor.set_threshold` | `threshold_mm` (5-500, mặc định 50) | dừng nếu cảm biến (hướng xuống) đọc từ giá trị này trở lên hoặc báo lỗi |
| `self.cliff_sensor.get_config` | — | |
| `self.cliff_sensor.test_now` | — | trả về 1 lần đọc khoảng cách (mm) |
| `self.led.set_mode` | `mode`: `follow_state`\|`both_on`\|`both_off`\|`left_only`\|`right_only` | `follow_state` giống hành vi LED trạng thái mặc định của các board khác |
| `self.led.get_mode` | — | |
| `self.arm.set_angle` / `self.neck.set_angle` | `angle` (0-180) | |
| `self.arm.wave` | — | cử chỉ vẫy tay dựng sẵn, không chặn luồng |
| `self.neck.turn` | `direction`: `left`\|`right`\|`center` | |
| `self.arm.release` / `self.neck.release` | — | ngắt PWM, servo mềm tự do |
| `self.screen.set_theme` | `theme`: `light`\|`dark`\|`ocean` | tool chung của `mcp_server.cc`; `ocean` là theme riêng của board này |
| `self.screen.set_orientation` | `orientation`: `portrait`\|`landscape` | lưu NVS, khởi động lại để áp dụng |
| `self.local_control.get_token` | — | trả về token ghép nối WS cục bộ hiện tại |
| `self.local_control.rotate_token` | — | tạo token mới và trả về, token cũ mất hiệu lực |

### Nguồn lệnh

Lệnh di chuyển có thể tới từ 1 trong 2 kênh (xem "Giao thức điều khiển" ở
trên) bất kỳ lúc nào, không có cơ chế phân xử nào ngoài "lệnh mới nhất
thắng, ngay lập tức" — không có khái niệm giữ quyền/khóa, nên app điện
thoại và phiên giọng nói có thể cùng điều khiển robot trong cùng khoảng
thời gian, ai gửi lệnh sau thì lệnh đó thắng. `self.wheelbot.get_control_status`
báo kênh nào (`local_ws` hay `cloud_voice`) vừa gửi lệnh di chuyển hiện
tại/gần nhất, để UI ít nhất hiển thị được khi *kênh kia* vừa giành quyền,
dù không thể giữ quyền thật sự. `command_source.h` gắn tag này cho mỗi
lệnh MCP đồng bộ; không đụng vào code nền tảng dùng chung
(`mcp_server.cc`, `application.cc`).

## Credit

Chân GPIO mặc định trong `config.h` (motor, ToF, LED, tay/cổ, cảm biến chạm,
mic, loa, màn hình) được căn theo sơ đồ đấu nối công khai và trang firmware
center của [KST AI Robot](https://ai.kenhsangtao.com/)
(`kenhsangtao.github.io/robotai`) — một robot ESP32-S3 thật, do cộng đồng
xây dựng, từ kênh "Kênh Sáng Tạo" (Việt Nam), có tính năng gần giống hệt
(driver động cơ chọn được, servo tay/cổ, chống rơi ToF, LED đôi, cảm biến
chạm, voice AI). Chỉ lấy **số GPIO** và **tên linh kiện công khai** từ trang
của họ; không lấy bất kỳ dòng code, hình ảnh, hay văn bản nào từ firmware
(một file binary đóng, không nêu license cho phép tái sử dụng) hay website
của họ. `wiring-diagram.svg` ở trên là sơ đồ gốc do dự án này tự vẽ lại từ
các dữ kiện công khai đó — không phải bản sao ảnh sơ đồ của họ. Firmware,
board definition, và MCP tool ở đây được viết độc lập.

## Chưa xác minh trên phần cứng thật

Toàn bộ mục dưới đây mới chỉ build-verified (compile sạch), **chưa có robot
thật nào chạy thử** — cần kiểm chứng trước khi tin tưởng hoàn toàn:

- **Quy ước chân L298N**: board này PWM trực tiếp trên `in1`/`in3` để điều
  khiển tốc độ, dùng `in2`/`in4` làm chân hướng tĩnh — **không** điều khiển
  2 chân enable ENA/ENB riêng của module chuẩn. Đây là kỹ thuật có thật, đã
  được tài liệu hóa (xem thread Raspberry Pi forums
  ["Using ENA or IN1/2 PWM for L298N motor speed control"](https://forums.raspberrypi.com/viewtopic.php?t=90243)),
  nhưng chỉ hoạt động nếu **jumper ENA/ENB trên module vẫn để nguyên**
  (nối cứng lên mức cao) — trạng thái mặc định từ nhà sản xuất trên hầu hết
  board L298N. Nếu module không có jumper đó, hoặc đã bị tháo, ENA/ENB phải
  nối cứng vào nguồn 5V/3.3V (hoặc phải mở rộng driver này để điều khiển
  luôn 2 chân đó) thì động cơ mới quay được. Riêng chuỗi ký tự trong
  firmware của KST AI Robot gợi ý "mini motor driver" của họ dùng mức tĩnh
  (không PWM) trực tiếp trên IN1-4 — một thiết kế module khác hẳn L298N
  chuẩn; chi tiết đó lấy từ binary string chứ không phải tài liệu công
  khai, nên coi là tín hiệu để kiểm tra module cụ thể của mình, không phải
  spec đã xác nhận. Kiểm tra lại với module thật trước khi tin
  `l298n_motor_driver.cc`.
- **Tốc độ servo xoay liên tục** (`servo_motor_driver.cc`): 1500µs dừng /
  dải 1000-2000µs full tốc độ khớp đúng chuẩn phổ biến, đã tài liệu hóa rộng
  rãi cho servo xoay liên tục (xem
  [dronebotworkshop.com/servoguide](https://dronebotworkshop.com/servoguide/),
  [hướng dẫn servo xoay liên tục của SparkFun](https://learn.sparkfun.com/tutorials/continuous-rotation-servo-trigger-hookup-guide/continuous-rotation-servo-motors)),
  không phải đoán bừa. Cái thực sự chưa xác minh là điểm dừng chính xác của
  *servo vật lý cụ thể* của anh — mỗi cái lệch nhau chút ít, đó chính là lý
  do mình expose nó thành tool chạy runtime (`self.wheelbot.set_servo_stop_pulse`)
  thay vì hằng số cố định; "nulling" servo thật bằng tay là bước hiệu chỉnh
  chuẩn ở đây, không phải lỗi cần sửa trong code.
- **Driver VL53L0X** (`vl53l0x.cc`): chỉ implement kiểm tra sự hiện diện +
  chuỗi trigger/poll/read đo khoảng cách đơn giản theo tài liệu. **Không**
  implement calibration đầy đủ của ST (SPAD, timing budget, signal-rate
  limit). Kiểm tra lại với khoảng cách đã biết trước khi dùng cho an toàn
  chống rơi.
- **Driver VL6180X** (`vl6180x.cc`): đã implement chuỗi khởi tạo bắt buộc
  ~30 thanh ghi riêng (SR03) theo AN4545 (đối chiếu chéo với
  pololu/vl6180x-arduino và Adafruit_VL6180X, đều MIT — chỉ lấy giá trị
  thanh ghi, không copy code), có kiểm tra cờ `SYSTEM_FRESH_OUT_OF_RESET`
  đúng theo datasheet yêu cầu. **Chưa** implement phần tinh chỉnh
  "recommended" riêng của AN4545 (readout averaging, ALS gain, interrupt
  config, VHV repeat rate) — những cái đó ảnh hưởng chất lượng đo, không
  phải việc đo có chạy được hay không. Vẫn chưa kiểm chứng trên phần cứng
  thật.
- **Hướng màn hình**: áp dụng bằng cách khởi động lại, không phải đổi layout
  ngay lập tức — vì gọi lại `esp_lcd_panel_swap_xy`/`mirror` lúc đang chạy
  mà không khởi động lại chưa được xác minh an toàn với cặp panel/driver
  này.
- **Cảm biến chạm TTP223**: đấu như một `Button` digital active-high đơn
  giản (`OnDoubleClick`) — module TTP223 giá rẻ đôi khi nhiễu/tự kích hoạt
  nếu nguồn 3V3 không ổn định; hành vi debounce chưa được test trên phần
  cứng thật.
