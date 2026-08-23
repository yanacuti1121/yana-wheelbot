#ifndef ARM_NECK_CONTROLLER_H
#define ARM_NECK_CONTROLLER_H

#include <string>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include "oscillator.h"

// Owns the arm and neck servos (plain 0-180 degree oscillator.h/.cc servos,
// copied from main/boards/otto-robot since that code is board-agnostic ledc
// PWM, not biped-specific) and registers their MCP tools.
//
// Concurrency note: MCP tool calls can arrive from either transport (local
// WebSocket or cloud/voice) on different FreeRTOS tasks, and self.arm.wave
// spawns its own short-lived task that keeps calling arm_.SetPosition() for
// ~1.5s. Without a guard, an set_angle/release call landing mid-wave (or two
// tool calls landing back to back on different tasks) would race on the
// same Oscillator's ledc state -- the same class of bug already fixed for
// motor control via WheelbotController::motor_mutex_. arm_mutex_/neck_mutex_
// apply that same fix here.
class ArmNeckController {
public:
    ArmNeckController(gpio_num_t arm_pin, gpio_num_t neck_pin);
    ~ArmNeckController();

    void SetArmAngle(int angle);
    void SetNeckAngle(int angle);
    void Wave();
    void TurnNeck(const std::string& direction);
    void ReleaseArm();
    void ReleaseNeck();

private:
    void RegisterMcpTools();

    Oscillator arm_;
    Oscillator neck_;
    gpio_num_t arm_pin_;
    gpio_num_t neck_pin_;
    // Oscillator::Attach() grabs a *new* ledc channel from a shared rotating
    // counter every time it's called, even if already attached — so these
    // flags exist to make sure Attach() is only called again after a real
    // Detach() (release), never unconditionally on every tool invocation.
    bool arm_attached_ = false;
    bool neck_attached_ = false;
    SemaphoreHandle_t arm_mutex_ = nullptr;
    SemaphoreHandle_t neck_mutex_ = nullptr;
};

#endif  // ARM_NECK_CONTROLLER_H
