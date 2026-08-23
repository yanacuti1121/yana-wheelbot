

# Yana Wheelbot Chassis

[Hướng dẫn in, BOM, đấu nối và lắp đặt bằng tiếng Việt](README_vi.md)

The chassis directory contains two parametric OpenSCAD fit-test decks:

- `yana_wheelbot_chassis_v2.scad` is the current integration prototype. It
main/boards/yana-wheelbot/chassis/README_vi.md
# Hướng dẫn in và lắp chassis Yana Wheelbot

Tài liệu này dành cho chassis:

- [`yana-wheelbot-chassis-final.stl`](yana-wheelbot-chassis-final.stl): file
  đem vào slicer để in.
- [`yana_wheelbot_chassis_v2.scad`](yana_wheelbot_chassis_v2.scad): nguồn
  OpenSCAD để chỉnh kích thước.
- Kích thước xuất STL hiện tại: **210 x 122 x 43,85 mm**.

Chassis là bản tích hợp chức năng, có vị trí cho ESP32-S3, màn hình, âm thanh,
cảm biến, bốn servo, pin và hai mạch nguồn. Đây chưa phải vỏ Wall-E trang trí.

> **Quan trọng:** kích thước PCB clone, pin, loa, switch và tai servo có thể
> khác listing. Trước khi in bản cuối, đo linh kiện thật bằng thước kẹp và sửa
> các biến ở đầu file `.scad` nếu sai lệch quá khoảng 0,5 mm.

## 1. Chọn vật liệu in

### Khuyến nghị: PETG cho bản sử dụng thật

PETG chịu va đập, rung và nhiệt trong thân robot tốt hơn PLA. Đây là lựa chọn
cân bằng nhất cho chassis chạy servo.

Cấu hình khởi điểm với nozzle 0,4 mm:

| Thông số | Giá trị khởi điểm |
|---|---:|
| Layer height | 0,20 mm |
| Thành vỏ | 4 perimeter/walls |
| Top/bottom | 5 lớp trở lên |
| Infill | 25–35% gyroid hoặc cubic |
| Nhiệt nozzle | 235–250 °C, theo cuộn nhựa |
| Nhiệt bàn | 75–85 °C, theo cuộn nhựa |
| Quạt | 30–50% |
| Brim | 5–8 mm nếu góc bị cong |

### PLA/PLA+

Dùng tốt cho bản thử lắp vì dễ in và ít cong. Không nên để robot PLA trong xe
ô tô, gần cửa kính nắng hoặc nguồn nhiệt. Cấu hình khởi điểm: 205–220 °C nozzle,
55–65 °C bàn, 4 walls và 25–30% infill.

### ASA/ABS

Chỉ nên dùng khi máy có buồng kín và người in đã quen kiểm soát co ngót. Phù
hợp nếu robot thường xuyên ở môi trường nóng. Sau khi in cần kiểm tra lại kích
thước pocket vì ASA/ABS co nhiều hơn PETG/PLA.

### TPU

Không dùng TPU cho deck chính hoặc gá servo. TPU chỉ phù hợp để in lốp, đệm
chống trượt, chân chống rung và miếng chèn dây.

### Không khuyến nghị

- Resin giòn cho gá chịu lực và vùng bắt servo.
- PLA silk cho chi tiết kết cấu.
- In quá ít thành vỏ; servo tạo tải rung lặp lại ở chân gá.

## 2. Hướng đặt và support

1. Đặt mặt phẳng lớn của deck nằm sát bàn in.
2. Không xoay chassis đứng cạnh; cách đó làm yếu gá servo và tốn support.
3. Bật support **chỉ từ bàn in** cho vùng bezel TFT nếu slicer báo cầu dài.
4. Kiểm tra preview từng lớp, đặc biệt tại bezel TFT, khe dây pin và gá servo.
5. Dùng brim nếu PETG/ABS cong bốn góc.
6. Sau in, bỏ support rồi thử từng linh kiện trước khi hàn dây.

Bốn lỗ bắt thân chính có đường kính khoảng 3,4 mm, phù hợp ốc M3 xuyên. Pocket
PCB hiện dùng thành giữ và rail đỡ, không giả định lỗ bắt vít của từng board.

## 3. BOM phần cứng chính

Link dưới đây là danh sách mua hàng riêng đã chọn cho dự án Yana, không lấy lại
link bán hàng của KST. Listing có thể đổi người bán, giá hoặc biến thể; phải
kiểm tra đúng thông số trước khi đặt hàng.

| # | Linh kiện | SL | Thông số cần mua | Nguồn | GPIO/bus | Kích thước CAD tham khảo | Link |
|---:|---|---:|---|---|---|---|---|
| 1 | ESP32-S3 N16R8 | 1 | WROOM-1, 16 MB flash, 8 MB PSRAM, DevKit 44-pin | 5 V/VIN; logic 3V3 | — | board khoảng 63–70 x 25–28 mm | [Coupang](https://www.coupang.com/vp/products/9476820138) |
| 2 | TFT ST7735 | 1 | 1,8 inch, 128x160, SPI | 3V3 | MOSI 11, SCK 12, DC 10, CS 13, RST 14 | khoảng 56 x 34 mm | [Coupang](https://www.coupang.com/vp/products/7991533839) |
| 3 | INMP441 | 1 | Micro MEMS digital I2S | 3V3 | WS 4, SCK 5, SD 6 | khoảng 15–20 x 10–15 mm | [Coupang](https://www.coupang.com/vp/products/8792464304?itemId=25592833527&vendorItemId=94254523602) |
| 4 | MAX98357A | 1 | I2S Class-D mono, khoảng 3 W | 5 V rail | LRC 15, BCLK 16, DIN 17 | khoảng 19 x 18 mm | [Coupang](https://www.coupang.com/vp/products/9649666019) |
| 5 | Loa | 1 | 3 W, 4 ohm | MAX98357A | — | đo đúng loa thực tế; CAD hiện chừa khoảng Ø42 mm | [Coupang](https://www.coupang.com/vp/products/9687303942?itemId=28970688668&vendorItemId=95932788332) |
| 6 | VL6180X | 1 | ToF I2C | 3V3 | SDA 1, SCL 2 | PCB khoảng 18–25 mm | [Coupang](https://www.coupang.com/vp/products/7206616775?itemId=18226765538&vendorItemId=85374303210) |
| 7 | TTP223 | 1 | Cảm ứng điện dung | 3V3 | OUT 7 | khoảng 14 x 11 mm | [Coupang](https://www.coupang.com/vp/products/9337846580?itemId=27690418458&vendorItemId=94652272760) |
| 8 | Servo 360° | 2 | Micro servo **continuous rotation** | 5 V servo rail | trái 47, phải 45 | SG90-class khoảng 23 x 12 x 29 mm | Chưa chốt URL; không mua nhầm bản 180° |
| 9 | Servo 180° | 2 | Micro servo positional cho tay và cổ | 5 V servo rail | tay 20, cổ 21 | khoảng 23 x 12 x 29 mm | [Coupang](https://www.coupang.com/vp/products/190778115?itemId=9577817213&vendorItemId=76862407783) |
| 10 | LED rời | 2 | Trắng hoặc warm-white, kèm điện trở hạn dòng | GPIO qua điện trở | trái 3, phải 18 | đo LED thực tế | [Coupang](https://www.coupang.com/vp/products/8202481908?itemId=23517797018&vendorItemId=90544172857) |
| 11 | LiPo | 1 | 1S 3,7 V, khoảng 2000 mAh, đủ dòng xả | — | — | đo đúng cell thực tế | [Coupang](https://www.coupang.com/vp/products/9430136637?itemId=28036955286&vendorItemId=92625474566) |
| 12 | TP4056 Type-C | 1 | Charger **có protection** cho cell 1S | USB 5 V | — | khoảng 25–30 x 17–20 mm | Chưa chốt URL |
| 13 | Boost converter | 1 | 3.x V lên 5 V ổn áp, loại đủ dòng cho bốn servo | LiPo qua switch | — | theo board thực tế; pocket CAD 42 x 24 mm | [Coupang](https://www.coupang.com/vp/products/9692004769?itemId=28987690226&vendorItemId=95916593087) |
| 14 | Mini ON/OFF switch | 1 | Latching slide switch, chịu đủ dòng đầu vào boost | đường pin | — | theo switch thực tế | [Coupang](https://www.coupang.com/vp/products/8758411816?itemId=25465322348&vendorItemId=95044703259) |

Không cần lắp L298N cho cấu hình này. Firmware có thể tiếp tục giữ backend
L298N, nhưng phần cứng đang dùng hai servo xoay liên tục.

## 4. Vật tư phụ

| Vật tư | Gợi ý sử dụng |
|---|---|
| Dây silicone 20–22 AWG | đường pin, boost và bus nguồn servo |
| Dây 26–28 AWG | tín hiệu GPIO, I2C, I2S |
| JST/connector khóa | pin, nguồn, loa và các cụm tháo rời |
| Header 2,54 mm | prototype các module |
| Perfboard hoặc power-distribution board | chia 5 V và GND sạch |
| Tụ bulk low-ESR | đặt gần bus servo; khởi điểm 1000–2200 µF, điện áp 6,3 V trở lên |
| Điện trở LED | tính theo LED; có thể bắt đầu thử ở 330 ohm |
| Heat-shrink | cách điện mối hàn |
| Ốc M3 | bốn lỗ bắt chassis Ø3,4 mm |
| Ốc servo và servo horn | bánh, tay và cổ |
| Dây đai hook-and-loop | giữ pin qua hai khe có sẵn |
| Cable tie nhỏ | giữ switch, dây và module nhẹ |
| Foam tape mỏng | chống rung; không dán lên antenna hoặc linh kiện nóng |
| USB-C data cable | flash và debug ESP32-S3 |

Không nên dùng Dupont làm kết nối cố định cho robot chuyển động. Dupont chỉ phù
hợp giai đoạn thử trên bàn; rung lâu ngày có thể làm lỏng chân.

## 5. Kiến trúc nguồn bắt buộc

```text
USB-C 5 V
    │
    ▼
TP4056 charger + protection
    │ B+/B- hoặc OUT+/OUT- theo đúng nhãn board
    ▼
LiPo 1S 3,0–4,2 V
    │
    ▼
ON/OFF switch
    │
    ▼
Boost ổn áp 5 V
    ├──────── ESP32-S3 chân 5V/VIN
    ├──────── MAX98357A VIN
    └──────── bốn servo VCC

ESP32-S3 chân 3V3
    ├──────── ST7735
    ├──────── INMP441
    ├──────── VL6180X
    └──────── TTP223

Tất cả GND nối chung.
```

Quy tắc an toàn:

- TP4056 là mạch sạc, **không phải boost**.
- Không cấp servo từ chân 3V3 của ESP32.
- Không nối pin 3,7 V trực tiếp vào bus 5 V.
- Không cấp 5 V vào chân 3V3.
- Boost phải đủ cả dòng liên tục và dòng đỉnh khi servo khởi động hoặc stall.
- Pin 2000 mAh chỉ nói dung lượng, không bảo đảm khả năng xả dòng.
- Bản đầu nên tắt robot khi sạc. TP4056 thông thường không có power-path/load
  sharing chuẩn để vừa chạy tải lớn vừa sạc.
- Đo đầu ra boost trước khi cắm ESP32. Chỉ nối khi điện áp ổn định gần 5 V.

## 6. Bảng đấu dây

### ST7735

| ST7735 | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| MOSI/SDA | GPIO11 |
| SCK/CLK | GPIO12 |
| DC/A0 | GPIO10 |
| CS | GPIO13 |
| RST/RES | GPIO14 |
| BL/LED | xem lưu ý dưới đây |

Firmware Yana Wheelbot hiện dùng **GPIO9** để điều khiển độ sáng. Nếu module
backlight tiêu thụ quá dòng GPIO cho phép, dùng transistor/MOSFET điều khiển
thay vì kéo LED nền trực tiếp từ GPIO. Cách đơn giản theo KST là nối BL vào 3V3,
nhưng khi đó không điều chỉnh được độ sáng bằng firmware.

### INMP441

| INMP441 | ESP32-S3 |
|---|---|
| VDD | 3V3 |
| GND | GND |
| L/R | GND |
| WS | GPIO4 |
| SCK/BCLK | GPIO5 |
| SD | GPIO6 |

Đưa lỗ mic hướng ra ngoài và tránh để keo bịt port âm thanh.

### MAX98357A và loa

| MAX98357A | Kết nối |
|---|---|
| VIN | bus 5 V từ boost |
| GND | GND chung |
| LRC/LRCLK | GPIO15 |
| BCLK | GPIO16 |
| DIN | GPIO17 |
| GAIN | GND theo cấu hình build sheet |
| SD | VIN theo cấu hình build sheet |
| SPK+ / SPK- | hai cực loa, không nối một cực loa xuống GND |

### VL6180X

| VL6180X | ESP32-S3 |
|---|---|
| VIN/VCC | 3V3 |
| GND | GND |
| SDA | GPIO1 |
| SCL | GPIO2 |

### TTP223

| TTP223 | ESP32-S3 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| OUT | GPIO7 |

### Servo

| Servo | Signal | Nguồn |
|---|---:|---|
| Bánh trái 360° | GPIO47 | 5 V servo rail + GND chung |
| Bánh phải 360° | GPIO45 | 5 V servo rail + GND chung |
| Tay 180° | GPIO20 | 5 V servo rail + GND chung |
| Cổ 180° | GPIO21 | 5 V servo rail + GND chung |

Thông thường dây servo nâu/đen là GND, đỏ là VCC và cam/vàng/trắng là signal,
nhưng phải kiểm tra datasheet hoặc nhãn của đúng servo.

### LED

| LED | GPIO |
|---|---:|
| Trái | GPIO3 |
| Phải | GPIO18 |

Mỗi LED phải có điện trở hạn dòng riêng. Không nối LED trực tiếp từ GPIO xuống
GND mà không có điện trở. LED công suất hoặc dòng lớn cần transistor driver.

### Chân không dùng

Không dùng GPIO36 và GPIO37 trên ESP32-S3 N16R8 của cấu hình này vì liên quan
tài nguyên PSRAM của module.

## 7. Bố trí linh kiện trên chassis

Quy ước trong file OpenSCAD: `+X` là phía trước, `+Y` là bên trái.

```text
                     PHÍA TRƯỚC (+X)
              [bezel TFT ST7735]

 [ToF nhìn xuống] [servo bánh] [ESP32-S3] [servo bánh]
 [mic] [touch] [amp] [boost] [TP4056]
 [servo cổ] [servo tay] [pin thấp] [loa + grille]

                     PHÍA SAU (-X)
```

- Pin nằm thấp, gần tâm và được giữ bằng dây đai.
- ESP32 để hở đầu USB-C; không dán antenna sát pin, nam châm loa hoặc boost.
- Loa đặt đúng trên grille, màng loa có đường thoát âm ra ngoài.
- Mic ở xa loa và amplifier để giảm hú/echo.
- ToF nhìn xuyên lỗ xuống sàn, nằm trước trục bánh để phát hiện mép sớm.
- TTP223 có thể nằm sau một lớp nhựa mỏng; không cần lộ PCB.
- Switch đặt vào hai khe đa dụng bằng cable tie hoặc adapter plate nhỏ.
- LED chưa có socket cố định vì kích thước package chưa được đo.

## 8. Thứ tự lắp cơ khí

1. In chassis và làm sạch support.
2. Thử khô toàn bộ linh kiện, chưa hàn và chưa dán.
3. Lắp hai servo bánh; trục phải hướng ra hai bên và cùng cao độ.
4. Gắn horn/bánh rồi quay thử bằng tay, bảo đảm không cạ deck hoặc dây.
5. Lắp servo tay và servo cổ; kiểm tra toàn bộ vùng quét 0–180°.
6. Lắp TFT từ phía mở của bezel, không ép vào kính hoặc cáp màn hình.
7. Đặt loa lên ring, mặt loa hướng về grille; không chọc thủng màng loa.
8. Đặt ToF và soi từ dưới để chắc chắn cửa quang không bị thành nhựa che.
9. Đặt ESP32, giữ đầu USB-C và vùng antenna thông thoáng.
10. Đặt TP4056, boost, MAX98357A, TTP223 và INMP441 vào đúng pocket.
11. Luồn dây đai qua hai khe rồi cố định pin; không siết làm biến dạng cell.
12. Đi dây nguồn trước, dây tín hiệu sau; tách dây mic khỏi dây loa và servo.
13. Tạo strain relief tại dây pin, dây loa và dây ra servo.
14. Chỉ đóng vỏ ngoài sau khi hoàn thành kiểm tra full-load.

## 9. Thứ tự đấu điện và chạy thử

### Giai đoạn A: nguồn

1. Kiểm tra đúng cực pin.
2. Nối pin với TP4056 đúng chân B+/B- hoặc theo nhãn board.
3. Nối output được bảo vệ qua switch tới input boost.
4. Chưa cắm ESP32 hay servo.
5. Bật switch và đo output boost.
6. Chỉnh boost về khoảng 5 V nếu board có biến trở.
7. Tắt nguồn và kiểm tra không chập 5 V/GND.

### Giai đoạn B: ESP32 và logic

1. Cấp 5 V vào chân 5V/VIN của ESP32.
2. Xác nhận ESP32 boot ổn.
3. Đo rail 3V3.
4. Gắn ST7735 và test màn hình.
5. Gắn INMP441 và test mic.
6. Gắn VL6180X rồi kiểm tra khoảng cách.
7. Gắn TTP223 và test chạm.

### Giai đoạn C: âm thanh và servo

1. Gắn MAX98357A, sau đó mới nối loa.
2. Test âm lượng thấp trước.
3. Test từng servo riêng, không gắn tải nặng.
4. Xác định điểm dừng của hai servo 360°.
5. Test lần lượt bánh trái, bánh phải, tay và cổ.
6. Cuối cùng mới chạy bốn servo cùng Wi-Fi, màn hình và âm thanh.

Nếu ESP32 reset khi servo chạy, ưu tiên kiểm tra boost, pin, tiết diện dây, đầu
nối và tụ bulk trước khi kết luận firmware lỗi.

## 10. Build và flash firmware Yana

Dùng ESP-IDF 6.0.2 khi có thể:

```sh
source /duong-dan-den/esp-idf/export.sh
idf.py --version
python3 scripts/build.py --list-boards
python3 scripts/build.py yana-wheelbot --name yana-wheelbot
```

Phần cứng trong tài liệu này dùng ST7735 và VL6180X. Bật hai lựa chọn sau trong
`idf.py menuconfig` tại mục **Yana Wheelbot** rồi build lại:

```text
CONFIG_YANA_WHEELBOT_DISPLAY_ST7735=y
CONFIG_YANA_WHEELBOT_TOF_VL6180X=y
```

Sau khi thay cấu hình:

```sh
idf.py fullclean
idf.py build
idf.py -p /dev/cu.usbmodemXXXX flash monitor
```

Thay `/dev/cu.usbmodemXXXX` bằng cổng serial thực tế. Dùng cáp USB-C có data.
Firmware KST chỉ là reference; bản Yana nên build từ source của repository này.

## 11. Phân biệt hướng dẫn KST và firmware Yana

Các thông tin công khai của KST chỉ được dùng làm **tham khảo phần cứng**: tên
linh kiện, GPIO, yêu cầu nguồn 5 V, GND chung và cảnh báo GPIO36/GPIO37. Không
sao chép firmware, nội dung hướng dẫn hoặc link tiếp thị của họ sang Yana.

| Nội dung KST reference | Hành vi đúng của Yana hiện tại |
|---|---|
| Web flasher, binary gộp và offset `0x0` | Build source Yana bằng ESP-IDF rồi flash bằng `idf.py`; không áp dụng offset KST cho firmware Yana |
| AP `KST-Robot-Ai-xxxx` và dashboard `/robot` | Không ghi thành cam kết của Yana; dùng luồng provisioning của firmware Yana và client trong `apps/controller-web` |
| Giữ TTP223 khi cấp nguồn khoảng 5 giây để đổi Wi-Fi | **Chưa được triển khai trong Yana Wheelbot**; TTP223 hiện chỉ nhận chạm đôi để bật/tắt trò chuyện |
| Kích hoạt tại `xiaozhi.me` và wake word “Hi Lily” | Phụ thuộc backend/cấu hình voice; không phải đặc tính cố định của chassis hoặc board Yana |
| ToF mặc định tắt | Yana hiện mặc định **bật** cliff sensor, ngưỡng 50 mm, và lưu cấu hình vào NVS |

Điều khiển cục bộ Yana dùng WebSocket tại
`ws://<IP>:8080/ws?token=<mã>` và cần pairing token. Xem
[controller-web](../../../../apps/controller-web/README.md) và tài liệu board
Yana Wheelbot ở thư mục cha.

## 12. Checklist trước khi chạy dưới sàn

- [ ] Pin không phồng, không thủng và đúng cực.
- [ ] TP4056 đúng loại 1S có protection.
- [ ] Boost ổn định gần 5 V khi có tải.
- [ ] Không chập 5 V/GND.
- [ ] Tất cả GND nối chung.
- [ ] ESP32 boot và không brownout.
- [ ] USB-C vẫn tiếp cận được.
- [ ] ST7735 hiển thị đúng chiều.
- [ ] Mic thu rõ, không bị keo che port.
- [ ] Loa không rè và màng loa không chạm chassis.
- [ ] Hai servo bánh dừng được ở lệnh stop.
- [ ] Chiều trái/phải đúng; hiệu chỉnh reverse nếu cần.
- [ ] Tay và cổ không đập vào chassis ở góc cuối hành trình.
- [ ] ToF nhìn xuống và không bị nhựa che.
- [ ] Bật cliff sensor rồi thử bằng cách giữ robot trên tay.
- [ ] Dây không chạm bánh, track hoặc horn.
- [ ] Pin, board nguồn và ESP32 tháo được để bảo trì.
- [ ] Full-load không làm ESP32 reset.

## 13. Lỗi thường gặp

| Hiện tượng | Kiểm tra trước |
|---|---|
| ESP32 reset khi servo chạy | boost thiếu dòng, pin xả yếu, dây nguồn nhỏ, đầu nối lỏng, thiếu tụ bulk |
| Servo rung khi đứng yên | GND chưa chung, nguồn nhiễu, signal đi sát dây công suất, điểm stop servo 360° lệch |
| Hai bánh chạy ngược nhau | dùng `self.wheelbot.set_servo_reverse`, không đảo cực nguồn servo |
| TFT trắng/đen | sai ST7735/ST7789 build option, sai CS/DC/RST, BL chưa cấp nguồn |
| Không có tiếng | sai BCLK/LRC/DIN, MAX98357A chưa có 5 V, đấu loa sai SPK+/SPK- |
| Mic nhiễu/hú | mic quá gần loa, dây mic chạy cạnh servo, âm lượng loa quá cao |
| ToF luôn báo lỗi | SDA/SCL đảo, không có GND chung, cửa quang bị chassis che, chưa bật VL6180X option |
| Wi-Fi yếu | antenna ESP32 sát pin, nam châm loa, boost hoặc vật kim loại |
| Chassis cong góc | tăng brim, vệ sinh bàn, giảm gió lùa, hiệu chỉnh nhiệt bàn |
| Linh kiện không lọt pocket | đo lại board thật và sửa envelope trong file `.scad` |

## 14. Bảo trì

- Kiểm tra định kỳ ốc servo, horn và dây đai pin.
- Không kéo dây để tháo JST; cầm vào housing connector.
- Ngắt pin trước khi sửa dây nguồn.
- Không sạc cell phồng, thủng hoặc nóng bất thường.
- Sau va chạm, kiểm tra nứt quanh gá servo và lỗ bắt chassis.
- Giữ lại file OpenSCAD đã chỉnh theo kích thước linh kiện thật để có thể in lại
  đúng phiên bản robot đang sử dụng.
