#include "motor_settings.h"

#include "settings.h"

namespace {
constexpr const char* kNamespace = "motor";
constexpr int kDefaultStopPulseUs = 1500;
}  // namespace

MotorSettingsData LoadMotorSettings(gpio_num_t default_in1, gpio_num_t default_in2,
                                    gpio_num_t default_in3, gpio_num_t default_in4) {
    Settings settings(kNamespace, true);
    MotorSettingsData data;
    data.type = settings.GetInt("type", 0) == 1 ? MotorType::kL298n : MotorType::kServo;
    data.in1 = static_cast<gpio_num_t>(settings.GetInt("in1", default_in1));
    data.in2 = static_cast<gpio_num_t>(settings.GetInt("in2", default_in2));
    data.in3 = static_cast<gpio_num_t>(settings.GetInt("in3", default_in3));
    data.in4 = static_cast<gpio_num_t>(settings.GetInt("in4", default_in4));
    data.stop_pulse_us = settings.GetInt("stop_us", kDefaultStopPulseUs);
    data.reverse_left = settings.GetBool("rev_l", false);
    data.reverse_right = settings.GetBool("rev_r", false);
    return data;
}

void SaveMotorTypeAndPins(MotorType type, gpio_num_t in1, gpio_num_t in2, gpio_num_t in3,
                          gpio_num_t in4) {
    Settings settings(kNamespace, true);
    settings.SetInt("type", type == MotorType::kL298n ? 1 : 0);
    settings.SetInt("in1", in1);
    settings.SetInt("in2", in2);
    settings.SetInt("in3", in3);
    settings.SetInt("in4", in4);
}

void SaveServoStopPulseUs(int stop_pulse_us) {
    Settings settings(kNamespace, true);
    settings.SetInt("stop_us", stop_pulse_us);
}

void SaveServoReverse(bool left, bool reversed) {
    Settings settings(kNamespace, true);
    settings.SetBool(left ? "rev_l" : "rev_r", reversed);
}
