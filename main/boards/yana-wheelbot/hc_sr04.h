#ifndef HC_SR04_H
#define HC_SR04_H

#include <driver/gpio.h>

#include "tof_sensor.h"

// HC-SR04 / US-100 ultrasonic distance sensor driver (alternate anti-fall
// sensor to VL53L0X/VL6180X, selected via Kconfig YANA_WHEELBOT_TOF_HCSR04).
// Cheaper and more widely available locally than either I2C ToF sensor, at
// the cost of a wider beam angle (less precise about exactly what's being
// measured) and a slower/noisier reading. US-100 in "jumper mode" (the
// jumper cap seated, per its own datasheet) speaks this exact same
// trigger/echo pulse protocol, so one driver covers both -- no jumper means
// UART mode, which this driver does not implement.
//
// Two GPIOs, not I2C: TRIG (output, 10us pulse starts a ranging cycle) and
// ECHO (input, stays high for a duration proportional to the round-trip
// time). Distance is derived from that pulse width and the speed of sound
// (343 m/s at ~20C, not temperature-compensated here), same textbook formula
// every maintained HC-SR04 driver uses -- there's no vendor register map to
// reference or copy since this is a timing protocol, not a digital
// interface.
class HcSr04 : public TofSensor {
public:
    HcSr04(gpio_num_t trig_pin, gpio_num_t echo_pin);

    // Triggers one measurement and converts the echo pulse width to
    // millimeters. Returns -1 if the echo pulse never starts or never ends
    // within kTimeoutUs (sensor unplugged, out of range, or wired wrong) --
    // fail-safe, matching TofSensor's documented contract.
    int ReadDistanceMm() override;

private:
    gpio_num_t trig_pin_;
    gpio_num_t echo_pin_;

    // Generous enough for HC-SR04's ~4m rated max range (~23ms round trip)
    // plus margin, without letting a disconnected sensor block the caller
    // for long -- this runs on CliffSensorController's own poll task, not a
    // shared one, so blocking it doesn't stall anything else.
    static constexpr int64_t kTimeoutUs = 35000;
};

#endif  // HC_SR04_H
