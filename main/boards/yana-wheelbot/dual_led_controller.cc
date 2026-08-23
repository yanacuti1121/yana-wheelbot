#include "dual_led_controller.h"

#include <esp_log.h>

#include "mcp_server.h"
#include "settings.h"

static const char* TAG = "DualLedController";
static constexpr const char* kNamespace = "led";

namespace {
LedMode ModeFromString(const std::string& s) {
    if (s == "both_on")
        return LedMode::kBothOn;
    if (s == "both_off")
        return LedMode::kBothOff;
    if (s == "left_only")
        return LedMode::kLeftOnly;
    if (s == "right_only")
        return LedMode::kRightOnly;
    return LedMode::kFollowState;
}

std::string ModeToString(LedMode mode) {
    switch (mode) {
        case LedMode::kBothOn:
            return "both_on";
        case LedMode::kBothOff:
            return "both_off";
        case LedMode::kLeftOnly:
            return "left_only";
        case LedMode::kRightOnly:
            return "right_only";
        default:
            return "follow_state";
    }
}
}  // namespace

DualLedController::DualLedController(gpio_num_t left_pin, gpio_num_t right_pin) {
    left_ = std::make_unique<GpioLed>(left_pin, 0, LEDC_TIMER_0, LEDC_CHANNEL_1);
    right_ = std::make_unique<GpioLed>(right_pin, 0, LEDC_TIMER_0, LEDC_CHANNEL_3);

    Settings settings(kNamespace, true);
    mode_ = ModeFromString(settings.GetString("mode", "follow_state"));
    ApplyManualMode();

    RegisterMcpTools();
}

void DualLedController::OnStateChanged() {
    if (mode_ != LedMode::kFollowState) {
        return;
    }
    left_->OnStateChanged();
    right_->OnStateChanged();
}

void DualLedController::ApplyManualMode() {
    switch (mode_) {
        case LedMode::kBothOn:
            left_->SetBrightness(100);
            right_->SetBrightness(100);
            left_->TurnOn();
            right_->TurnOn();
            break;
        case LedMode::kBothOff:
            left_->TurnOff();
            right_->TurnOff();
            break;
        case LedMode::kLeftOnly:
            left_->SetBrightness(100);
            left_->TurnOn();
            right_->TurnOff();
            break;
        case LedMode::kRightOnly:
            right_->SetBrightness(100);
            right_->TurnOn();
            left_->TurnOff();
            break;
        case LedMode::kFollowState:
            // Let the next OnStateChanged() call (already wired via
            // Board::GetLed(), see Application::HandleStateChangedEvent())
            // drive both LEDs.
            break;
    }
}

void DualLedController::SetMode(LedMode mode) {
    mode_ = mode;
    Settings settings(kNamespace, true);
    settings.SetString("mode", ModeToString(mode));
    ApplyManualMode();
}

void DualLedController::RegisterMcpTools() {
    auto& mcp = McpServer::GetInstance();

    mcp.AddTool(
        "self.led.set_mode",
        "Set LED mode: follow_state (auto color/blink from device state), both_on, both_off, "
        "left_only, or right_only.",
        PropertyList({Property("mode", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            SetMode(ModeFromString(p["mode"].value<std::string>()));
            return true;
        });

    mcp.AddTool("self.led.get_mode", "Get the current LED mode.", PropertyList(),
                [this](const PropertyList&) -> ReturnValue { return ModeToString(mode_); });
}
