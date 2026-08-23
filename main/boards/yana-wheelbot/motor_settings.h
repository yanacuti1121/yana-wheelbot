#ifndef MOTOR_SETTINGS_H
#define MOTOR_SETTINGS_H

#include <driver/gpio.h>

// NVS-backed motor configuration, modeled directly on
// main/boards/common/dual_network_board.cc's Settings("network", true) +
// reboot-to-apply pattern. Namespace: "motor".
enum class MotorType {
    kServo = 0,
    kL298n = 1,
};

struct MotorSettingsData {
    MotorType type;
    gpio_num_t in1;
    gpio_num_t in2;
    gpio_num_t in3;
    gpio_num_t in4;
    int stop_pulse_us;
    bool reverse_left;
    bool reverse_right;
};

// Loads persisted motor settings, falling back to the given compile-time
// defaults for any key not yet written.
MotorSettingsData LoadMotorSettings(gpio_num_t default_in1, gpio_num_t default_in2,
                                    gpio_num_t default_in3, gpio_num_t default_in4);

// Persists a new motor type + pin mapping. Caller must reboot to apply, since
// swapping GPIO ownership / ledc channel assignment at runtime is not safe.
void SaveMotorTypeAndPins(MotorType type, gpio_num_t in1, gpio_num_t in2, gpio_num_t in3,
                          gpio_num_t in4);

// Live-appliable calibration (no reboot required).
void SaveServoStopPulseUs(int stop_pulse_us);
void SaveServoReverse(bool left, bool reversed);

#endif  // MOTOR_SETTINGS_H
