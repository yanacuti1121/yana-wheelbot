#include "servo_motor_driver.h"

#include <algorithm>

#include <esp_log.h>

static const char* TAG = "ServoMotorDriver";

// Same global pulse-width bounds used by main/boards/otto-robot/oscillator.h.
static constexpr int kMinPulseUs = 500;
static constexpr int kMaxPulseUs = 2500;
static constexpr int kPeriodUs = 20000;     // 50Hz
static constexpr uint32_t kMaxDuty = 8191;  // 13-bit

ServoMotorDriver::ServoMotorDriver(gpio_num_t left_pin, gpio_num_t right_pin, int stop_pulse_us,
                                   bool reverse_left, bool reverse_right)
    : stop_pulse_us_(stop_pulse_us) {
    left_.reverse = reverse_left;
    right_.reverse = reverse_right;

    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                      .duty_resolution = LEDC_TIMER_13_BIT,
                                      .timer_num = LEDC_TIMER_2,
                                      .freq_hz = 50,
                                      .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    AttachChannel(left_, left_pin, LEDC_CHANNEL_5);
    AttachChannel(right_, right_pin, LEDC_CHANNEL_6);

    Stop();
}

ServoMotorDriver::~ServoMotorDriver() {
    ledc_stop(LEDC_LOW_SPEED_MODE, left_.ledc_channel, 0);
    ledc_stop(LEDC_LOW_SPEED_MODE, right_.ledc_channel, 0);
}

void ServoMotorDriver::AttachChannel(Channel& channel, gpio_num_t pin,
                                     ledc_channel_t ledc_channel) {
    channel.pin = pin;
    channel.ledc_channel = ledc_channel;

    ledc_channel_config_t cfg = {.gpio_num = pin,
                                 .speed_mode = LEDC_LOW_SPEED_MODE,
                                 .channel = ledc_channel,
                                 .intr_type = LEDC_INTR_DISABLE,
                                 .timer_sel = LEDC_TIMER_2,
                                 .duty = 0,
                                 .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&cfg));
}

void ServoMotorDriver::WritePulse(const Channel& channel, int speed) {
    speed = std::clamp(speed, -100, 100);
    if (channel.reverse) {
        speed = -speed;
    }

    int pulse_us = stop_pulse_us_ + (speed * kHalfRangeUs) / 100;
    pulse_us = std::clamp(pulse_us, kMinPulseUs, kMaxPulseUs);

    uint32_t duty = static_cast<uint32_t>((static_cast<int64_t>(pulse_us) * kMaxDuty) / kPeriodUs);
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, channel.ledc_channel, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, channel.ledc_channel));
}

void ServoMotorDriver::Drive(int left_speed, int right_speed) {
    WritePulse(left_, left_speed);
    WritePulse(right_, right_speed);
}

void ServoMotorDriver::Stop() {
    WritePulse(left_, 0);
    WritePulse(right_, 0);
}

void ServoMotorDriver::SetStopPulseUs(int stop_pulse_us) {
    stop_pulse_us_ = std::clamp(stop_pulse_us, kMinPulseUs, kMaxPulseUs);
    ESP_LOGI(TAG, "stop pulse set to %d us", stop_pulse_us_);
}

void ServoMotorDriver::SetReverse(bool left, bool reversed) {
    if (left) {
        left_.reverse = reversed;
    } else {
        right_.reverse = reversed;
    }
}
