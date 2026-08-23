#ifndef SERVO_MOTOR_DRIVER_H
#define SERVO_MOTOR_DRIVER_H

#include <driver/gpio.h>
#include <driver/ledc.h>

#include "motor_driver.h"

// Drives two continuous-rotation ("360-degree") hobby servos as a differential
// wheel pair, using the same ledc PWM mechanism as main/boards/otto-robot's
// Oscillator class (50Hz, 13-bit duty), but mapping speed+direction around a
// configurable stop-pulse center instead of an absolute 0-180 angle.
//
// NOTE: the exact microseconds-per-percent-speed slope has not been verified
// against real hardware in this codebase (no continuous-rotation servo code
// existed here before). Tune `half_range_us_` via testing if the robot moves
// too slowly/quickly near the stop point.
class ServoMotorDriver : public MotorDriver {
public:
    ServoMotorDriver(gpio_num_t left_pin, gpio_num_t right_pin, int stop_pulse_us,
                     bool reverse_left, bool reverse_right);
    ~ServoMotorDriver() override;

    void Drive(int left_speed, int right_speed) override;
    void Stop() override;

    // Live-appliable calibration (no reboot needed).
    void SetStopPulseUs(int stop_pulse_us);
    void SetReverse(bool left, bool reversed);

private:
    struct Channel {
        gpio_num_t pin;
        ledc_channel_t ledc_channel;
        bool reverse;
    };

    void AttachChannel(Channel& channel, gpio_num_t pin, ledc_channel_t ledc_channel);
    void WritePulse(const Channel& channel, int speed);

    Channel left_;
    Channel right_;
    int stop_pulse_us_;
    static constexpr int kHalfRangeUs = 500;  // stays within SERVO_MIN/MAX 500-2500us
};

#endif  // SERVO_MOTOR_DRIVER_H
