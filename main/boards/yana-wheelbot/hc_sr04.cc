#include "hc_sr04.h"

#include <esp_timer.h>
#include <rom/ets_sys.h>

HcSr04::HcSr04(gpio_num_t trig_pin, gpio_num_t echo_pin)
    : trig_pin_(trig_pin), echo_pin_(echo_pin) {
    gpio_config_t trig_cfg = {
        .pin_bit_mask = 1ULL << trig_pin_,
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&trig_cfg);
    gpio_set_level(trig_pin_, 0);

    gpio_config_t echo_cfg = {
        .pin_bit_mask = 1ULL << echo_pin_,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&echo_cfg);
}

int HcSr04::ReadDistanceMm() {
    // 10us trigger pulse -- every HC-SR04/US-100 datasheet's minimum.
    gpio_set_level(trig_pin_, 1);
    ets_delay_us(10);
    gpio_set_level(trig_pin_, 0);

    int64_t start = esp_timer_get_time();
    while (gpio_get_level(echo_pin_) == 0) {
        if (esp_timer_get_time() - start > kTimeoutUs) {
            return -1;  // echo never started -- unplugged or miswired
        }
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(echo_pin_) == 1) {
        if (esp_timer_get_time() - echo_start > kTimeoutUs) {
            return -1;  // echo never ended -- out of range or miswired
        }
    }
    int64_t pulse_us = esp_timer_get_time() - echo_start;

    // distance_mm = pulse_us * speed_of_sound_mm_per_us / 2 (round trip).
    // Speed of sound at ~20C: 343 m/s = 0.343 mm/us.
    return static_cast<int>(pulse_us * 0.343 / 2.0);
}
