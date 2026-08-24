# Fire-3D-CAD

Firmware và CAD cho bản chuẩn wheelbot.

- `KST-AI-Robot-ST7735.bin`, `yana-wheelbot-firmware.bin` — firmware, để nguyên trên git.
- `*.3mf.enc` — bản CAD gốc (`.3mf`) đã mã hóa AES-256-CBC (PBKDF2, 200000 iter) vì
  liên quan bản quyền, không được công khai. File gốc `.3mf` chỉ tồn tại local, đã
  bị `.gitignore` chặn (`Fire-3D-CAD/*.3mf`).

## Giải mã lại khi cần

```bash
openssl enc -d -aes-256-cbc -pbkdf2 -iter 200000 \
  -in "ten-file.3mf.enc" -out "ten-file.3mf" -pass "pass:<passphrase>"
```

Passphrase không lưu trong repo — lấy từ password manager của anh Tâm.
