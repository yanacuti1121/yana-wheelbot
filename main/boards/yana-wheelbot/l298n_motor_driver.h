#ifndef L298N_MOTOR_DRIVER_H
#define L298N_MOTOR_DRIVER_H

#include <driver/gpio.h>
#include <driver/ledc.h>

#include "motor_driver.h"

// Drives two DC gear motors through an L298N H-bridge module.
//
// This PWMs in1/in3 directly for speed and uses in2/in4 as static direction
// pins -- it does NOT drive the standard module's separate ENA/ENB enable
// pins. This is a real, documented technique (PWM-on-IN instead of
// PWM-on-EN), but it only works if your module's ENA/ENB jumpers are LEFT
// INSTALLED (tying them permanently high, the factory-default state on most
// breakouts). If your module lacks those jumpers or they've been removed,
// ENA/ENB must be tied to a permanent 5V/3.3V source, or this driver
// extended to also drive them, before the motors will spin at all. See
// this board's README "Not yet verified on real hardware" section for the
// full caveat (including a different, non-PWM static-logic module design
// hinted at by the KST AI Robot's firmware strings -- not this scheme).
class L298nMotorDriver : public MotorDriver {
public:
    L298nMotorDriver(gpio_num_t in1, gpio_num_t in2, gpio_num_t in3, gpio_num_t in4);
    ~L298nMotorDriver() override;

    void Drive(int left_speed, int right_speed) override;
    void Stop() override;

private:
    void ConfigurePwmPin(gpio_num_t pin, ledc_channel_t channel, ledc_timer_t timer);
    void WriteSide(gpio_num_t pwm_pin, ledc_channel_t pwm_channel, gpio_num_t dir_pin, int speed);

    gpio_num_t in1_, in2_, in3_, in4_;
};

#endif  // L298N_MOTOR_DRIVER_H
