#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// GPIO defaults below are aligned with the KST AI Robot's publicly published
// wiring diagram (ai.kenhsangtao.com/docs/So-do-dau-noi-ST7735.svg) -- a
// real, community-tested ESP32-S3 robot with the same feature set (dual
// motor backend, arm/neck servos, ToF anti-fall, dual LED, touch sensor).
// Only the pin *numbers* are taken from that public diagram; no firmware
// code was copied from their (closed-source, no reuse license stated)
// binary. Every pin below matches their diagram exactly, including
// audio_i2s_spk_gpio_dout (MAX98357A DIN -> GPIO17) and touch_sensor_pin
// (TTP223 OUT -> GPIO7), both confirmed from the diagram image (previously
// unconfirmed guesses in earlier commits). display_backlight_pin is NOT in
// their diagram -- their ST7735 module ties BL straight to 3V3 (no
// software control) -- so it remains this project's own free-pin pick, now
// moved off GPIO17 since that pin is confirmed in use for audio DIN.
// Runtime-remappable pins (motor IN1-4, servo stop-pulse/reverse) are
// stored in NVS via Settings and can be changed from the app without
// re-flashing; the constants here are only the first-boot defaults. Verify
// against your actual wiring before relying on them.

struct HardwareConfig {
    // Motor / servo drive (defaults; motor type + IN1-4 pins are overridable at
    // runtime via the "motor" Settings namespace, see motor_settings.h)
    gpio_num_t motor_in1_pin;    // L298N IN1 (left PWM/EN)
    gpio_num_t motor_in2_pin;    // L298N IN2 (left DIR)
    gpio_num_t motor_in3_pin;    // L298N IN3 (right PWM/EN)
    gpio_num_t motor_in4_pin;    // L298N IN4 (right DIR)
    gpio_num_t servo_left_pin;   // continuous-rotation servo, left wheel
    gpio_num_t servo_right_pin;  // continuous-rotation servo, right wheel

    // ToF anti-fall sensor (VL53L0X/VL6180X, I2C)
    gpio_num_t tof_i2c_sda_pin;
    gpio_num_t tof_i2c_scl_pin;

    // Anti-fall sensor alternative: HC-SR04/US-100 ultrasonic (GPIO
    // trigger/echo timing, no I2C bus). Only used when
    // CONFIG_YANA_WHEELBOT_TOF_HCSR04 is enabled -- otherwise unused, and
    // these two pins stay free for anything else.
    gpio_num_t tof_trig_pin;
    gpio_num_t tof_echo_pin;

    // LEDs
    gpio_num_t led_left_pin;
    gpio_num_t led_right_pin;

    // Arm / neck servos
    gpio_num_t arm_servo_pin;
    gpio_num_t neck_servo_pin;

    // Touch sensor (TTP223), digital active-high output. Double-tap toggles
    // chat, mirroring the boot button's OnClick and the KST AI Robot's own
    // double-tap-to-chat behavior.
    gpio_num_t touch_sensor_pin;

    // Audio (I2S simplex: shared bus, separate mic/speaker pin sets)
    int audio_input_sample_rate;
    int audio_output_sample_rate;
    gpio_num_t audio_i2s_mic_gpio_ws;
    gpio_num_t audio_i2s_mic_gpio_sck;
    gpio_num_t audio_i2s_mic_gpio_din;
    gpio_num_t audio_i2s_spk_gpio_dout;
    gpio_num_t audio_i2s_spk_gpio_bclk;
    gpio_num_t audio_i2s_spk_gpio_lrck;

    // Display (SPI ST7789)
    gpio_num_t display_backlight_pin;
    gpio_num_t display_mosi_pin;
    gpio_num_t display_clk_pin;
    gpio_num_t display_dc_pin;
    gpio_num_t display_rst_pin;
    gpio_num_t display_cs_pin;
};

constexpr HardwareConfig WHEELBOT_HARDWARE_CONFIG = {
    // Matches KST wiring diagram exactly.
    .motor_in1_pin = GPIO_NUM_38,
    .motor_in2_pin = GPIO_NUM_39,
    .motor_in3_pin = GPIO_NUM_40,
    .motor_in4_pin = GPIO_NUM_41,
    .servo_left_pin = GPIO_NUM_47,   // KST: left wheel servo
    .servo_right_pin = GPIO_NUM_45,  // KST: right wheel servo

    // Matches KST wiring diagram exactly (I2C for VL6180X/TOF050C on their board).
    .tof_i2c_sda_pin = GPIO_NUM_1,
    .tof_i2c_scl_pin = GPIO_NUM_2,

    // HC-SR04/US-100 alternative (only wired up if selected in Kconfig).
    // GPIO8/19 aren't used by anything else in this board's pin map --
    // verify against your actual wiring before relying on them.
    .tof_trig_pin = GPIO_NUM_8,
    .tof_echo_pin = GPIO_NUM_19,

    // Matches KST wiring diagram exactly.
    .led_left_pin = GPIO_NUM_3,
    .led_right_pin = GPIO_NUM_18,

    // Matches KST wiring diagram exactly.
    .arm_servo_pin = GPIO_NUM_20,
    .neck_servo_pin = GPIO_NUM_21,

    // Matches KST wiring diagram exactly (TTP223 OUT).
    .touch_sensor_pin = GPIO_NUM_7,

    // Mic pins match KST wiring diagram exactly (INMP441 on their board).
    .audio_input_sample_rate = 16000,
    .audio_output_sample_rate = 24000,
    .audio_i2s_mic_gpio_ws = GPIO_NUM_4,
    .audio_i2s_mic_gpio_sck = GPIO_NUM_5,
    .audio_i2s_mic_gpio_din = GPIO_NUM_6,
    // Matches KST wiring diagram exactly (MAX98357A: DIN/LRC/BCLK).
    .audio_i2s_spk_gpio_dout = GPIO_NUM_17,
    .audio_i2s_spk_gpio_bclk = GPIO_NUM_16,
    .audio_i2s_spk_gpio_lrck = GPIO_NUM_15,

    // Display SPI pins (MOSI/CLK/DC/CS/RST) match KST's diagram; their board
    // uses an ST7735 panel, this one uses ST7789 -- same 5 SPI signal roles,
    // different panel driver/init sequence in yana_wheelbot_board.cc.
    // display_backlight_pin isn't in their diagram -- their ST7735 module
    // ties BL straight to 3V3, no GPIO control -- so GPIO_NUM_9 here is our
    // own free-pin pick (this board keeps software backlight control as a
    // feature), not confirmed against their hardware.
    .display_backlight_pin = GPIO_NUM_9,
    .display_mosi_pin = GPIO_NUM_11,
    .display_clk_pin = GPIO_NUM_12,
    .display_dc_pin = GPIO_NUM_10,
    .display_rst_pin = GPIO_NUM_14,
    .display_cs_pin = GPIO_NUM_13,
};

// Portrait 128x160 is the default orientation; landscape swaps width/height and
// sets swap_xy. Both are applied at boot from the persisted "display" Settings
// namespace (see yana_wheelbot_board.cc), never re-applied live without a reboot.
#define DISPLAY_PORTRAIT_WIDTH 128
#define DISPLAY_PORTRAIT_HEIGHT 160
#define DISPLAY_MIRROR_X false
#define DISPLAY_MIRROR_Y false
#define DISPLAY_INVERT_COLOR true
#define DISPLAY_RGB_ORDER LCD_RGB_ELEMENT_ORDER_RGB
#define DISPLAY_OFFSET_X 0
#define DISPLAY_OFFSET_Y 0
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false
#define DISPLAY_SPI_MODE 3

#define BOOT_BUTTON_GPIO GPIO_NUM_0

// VL53L0X default 7-bit I2C address.
#define VL53L0X_I2C_ADDR 0x29

// VL6180X default 7-bit I2C address (same default as VL53L0X, coincidentally).
#define VL6180X_I2C_ADDR 0x29

#endif  // _BOARD_CONFIG_H_
