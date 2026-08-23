#ifndef DUAL_LED_CONTROLLER_H
#define DUAL_LED_CONTROLLER_H

#include <memory>

#include <driver/gpio.h>

#include "led/gpio_led.h"

enum class LedMode {
    kFollowState = 0,
    kBothOn = 1,
    kBothOff = 2,
    kLeftOnly = 3,
    kRightOnly = 4,
};

// Wraps two plain-GPIO LEDs (left/right) as a single Led, adding the one
// capability none of main/led/*'s existing Led subclasses have: a manual/
// auto-follow mode switch. In kFollowState, OnStateChanged() (already called
// automatically by Application on every DeviceState transition — see
// main/application.cc's HandleStateChangedEvent()) drives both LEDs using the
// same per-state brightness/blink mapping as GpioLed::OnStateChanged(). In any
// manual mode, OnStateChanged() is a no-op so the manual setting sticks until
// the mode is changed again. Settings namespace: "led" (key "mode").
class DualLedController : public Led {
public:
    DualLedController(gpio_num_t left_pin, gpio_num_t right_pin);

    void OnStateChanged() override;

    void SetMode(LedMode mode);
    LedMode GetMode() const { return mode_; }

private:
    void RegisterMcpTools();
    void ApplyManualMode();

    std::unique_ptr<GpioLed> left_;
    std::unique_ptr<GpioLed> right_;
    LedMode mode_ = LedMode::kFollowState;
};

#endif  // DUAL_LED_CONTROLLER_H
