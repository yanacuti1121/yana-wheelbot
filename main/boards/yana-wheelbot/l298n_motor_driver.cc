#include "l298n_motor_driver.h"

#include <algorithm>
#include <cmath>

#include <esp_log.h>

static const char* TAG = "L298nMotorDriver";

static constexpr uint32_t kMaxDuty = 8191;  // 13-bit, matches the servo driver's resolution

L298nMotorDriver::L298nMotorDriver(gpio_num_t in1, gpio_num_t in2, gpio_num_t in3, gpio_num_t in4)
    : in1_(in1), in2_(in2), in3_(in3), in4_(in4) {
    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                      .duty_resolution = LEDC_TIMER_13_BIT,
                                      .timer_num = LEDC_TIMER_3,
                                      .freq_hz = 2000,  // typical L298N-safe PWM frequency
                                      .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    ConfigurePwmPin(in1_, LEDC_CHANNEL_7, LEDC_TIMER_3);

    gpio_config_t dir_cfg = {};
    dir_cfg.pin_bit_mask = (1ULL << in2_) | (1ULL << in4_);
    dir_cfg.mode = GPIO_MODE_OUTPUT;
    dir_cfg.pull_up_en = GPIO_PULLUP_DISABLE;
    dir_cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
    dir_cfg.intr_type = GPIO_INTR_DISABLE;
    ESP_ERROR_CHECK(gpio_config(&dir_cfg));

    // in3 shares a second PWM channel on the same timer.
    ledc_channel_config_t in3_cfg = {.gpio_num = in3_,
                                     .speed_mode = LEDC_LOW_SPEED_MODE,
                                     .channel = LEDC_CHANNEL_0,
                                     .intr_type = LEDC_INTR_DISABLE,
                                     .timer_sel = LEDC_TIMER_3,
                                     .duty = 0,
                                     .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&in3_cfg));

    Stop();
}

L298nMotorDriver::~L298nMotorDriver() { Stop(); }

void L298nMotorDriver::ConfigurePwmPin(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer) {
    ledc_channel_config_t cfg = {.gpio_num = pin,
                                 .speed_mode = LEDC_LOW_SPEED_MODE,
                                 .channel = channel,
                                 .intr_type = LEDC_INTR_DISABLE,
                                 .timer_sel = timer,
                                 .duty = 0,
                                 .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&cfg));
}

void L298nMotorDriver::WriteSide(gpio_num_t pwm_pin, ledc_channel_t pwm_channel, gpio_num_t dir_pin,
                                 int speed) {
    speed = std::clamp(speed, -100, 100);
    gpio_set_level(dir_pin, speed >= 0 ? 1 : 0);

    uint32_t duty = static_cast<uint32_t>((std::abs(speed) * kMaxDuty) / 100);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, pwm_channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, pwm_channel));
}

void L298nMotorDriver::Drive(int left_speed, int right_speed) {
    WriteSide(in1_, LEDC_CHANNEL_7, in2_, left_speed);
    WriteSide(in3_, LEDC_CHANNEL_0, in4_, right_speed);
}

void L298nMotorDriver::Stop() {
    WriteSide(in1_, LEDC_CHANNEL_7, in2_, 0);
    WriteSide(in3_, LEDC_CHANNEL_0, in4_, 0);
}
