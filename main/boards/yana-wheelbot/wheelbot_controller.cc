#include "wheelbot_controller.h"

#include <esp_log.h>
#include <cJSON.h>

#include "application.h"
#include "config.h"
#include "l298n_motor_driver.h"
#include "mcp_server.h"

static const char* TAG = "WheelbotController";

WheelbotController::WheelbotController(gpio_num_t default_in1, gpio_num_t default_in2,
                                       gpio_num_t default_in3, gpio_num_t default_in4) {
    settings_ = LoadMotorSettings(default_in1, default_in2, default_in3, default_in4);
    BuildMotorDriver();

    motor_mutex_ = xSemaphoreCreateMutex();
    if (motor_mutex_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create motor mutex -- motor control disabled");
    }

    action_queue_ = xQueueCreate(4, sizeof(ActionParams));
    if (action_queue_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create action queue -- motor control disabled");
    } else if (xTaskCreate(ActionTask, "WheelbotAction", 3072, this, configMAX_PRIORITIES - 2,
                           &action_task_handle_) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create action task -- motor control disabled");
        vQueueDelete(action_queue_);
        action_queue_ = nullptr;
    }

    RegisterMcpTools();
}

WheelbotController::~WheelbotController() {
    if (action_task_handle_ != nullptr) {
        vTaskDelete(action_task_handle_);
    }
    if (action_queue_ != nullptr) {
        vQueueDelete(action_queue_);
    }
    if (motor_mutex_ != nullptr) {
        vSemaphoreDelete(motor_mutex_);
    }
}

void WheelbotController::BuildMotorDriver() {
    if (settings_.type == MotorType::kServo) {
        auto* driver = new ServoMotorDriver(
            WHEELBOT_HARDWARE_CONFIG.servo_left_pin, WHEELBOT_HARDWARE_CONFIG.servo_right_pin,
            settings_.stop_pulse_us, settings_.reverse_left, settings_.reverse_right);
        servo_driver_ = driver;
        motor_driver_.reset(driver);
        ESP_LOGI(TAG, "Motor backend: continuous-rotation servo pair");
    } else {
        motor_driver_.reset(
            new L298nMotorDriver(settings_.in1, settings_.in2, settings_.in3, settings_.in4));
        servo_driver_ = nullptr;
        ESP_LOGI(TAG, "Motor backend: L298N (in1=%d in2=%d in3=%d in4=%d)", settings_.in1,
                 settings_.in2, settings_.in3, settings_.in4);
    }
}

namespace {
// ESP32-S3-WROOM-1 N16R8 reserved GPIOs, verified against ESP-IDF's own
// SOC_GPIO_VALID_GPIO_MASK (esp32s3/include/soc/soc_caps.h -- GPIO22-25
// don't physically exist on this chip) and Espressif's datasheet (GPIO26-32:
// shared SPI flash/PSRAM bus on every ESP32-S3 variant; GPIO33-37:
// additional Octal PSRAM pins on R8+ modules specifically, this board's MCU).
// Strapping pins (0, 3, 45, 46) are also rejected here -- not physically
// impossible to use, but risky for an actively-driven motor-control pin
// (see board README's "Not yet verified" section), so kept out of the
// remappable range rather than merely warned about.
bool IsPinUsableForMotor(gpio_num_t pin) {
    int n = static_cast<int>(pin);
    if (n < 0 || n > 48) return false;
    if (n >= 22 && n <= 25) return false;  // does not exist on ESP32-S3
    if (n >= 26 && n <= 37) return false;  // flash/PSRAM (26-32 always, 33-37 on R8+)
    if (n == 0 || n == 3 || n == 45 || n == 46) return false;  // strapping pins
    return true;
}
}  // namespace

const char* WheelbotController::ValidateMotorPins(gpio_num_t in1, gpio_num_t in2, gpio_num_t in3,
                                                  gpio_num_t in4) {
    gpio_num_t pins[4] = {in1, in2, in3, in4};
    for (int i = 0; i < 4; i++) {
        if (!IsPinUsableForMotor(pins[i])) {
            return "One or more pins is reserved (flash/PSRAM, nonexistent, or a strapping pin) "
                   "or out of range 0-48.";
        }
        for (int j = i + 1; j < 4; j++) {
            if (pins[i] == pins[j]) {
                return "in1-in4 must all be distinct GPIOs.";
            }
        }
    }
    return nullptr;
}

bool WheelbotController::QueueMove(ActionType action, int speed, int duration_ms) {
    if (action_queue_ == nullptr || movement_inhibited_) {
        return false;
    }
    // g_yana_command_source reflects whichever transport is synchronously
    // calling this right now (see command_source.h) -- recorded here so
    // self.wheelbot.get_control_status can report it.
    active_source_ = g_yana_command_source;
    active_action_type_ = static_cast<int>(action);
    ActionParams params{static_cast<int>(action), speed, duration_ms, g_yana_command_source};
    // Drop any queued-or-in-flight move; the newest command wins immediately
    // (ActionTask's wait loop below also checks for a fresh queued item and
    // breaks out of whatever move is currently running, rather than only
    // discarding moves that hadn't started yet).
    xQueueReset(action_queue_);
    return xQueueSend(action_queue_, &params, 0) == pdTRUE;
}

void WheelbotController::EmergencyStop() {
    stop_requested_ = true;
    if (action_queue_ != nullptr) {
        xQueueReset(action_queue_);
    }
    if (motor_mutex_ != nullptr) {
        xSemaphoreTake(motor_mutex_, portMAX_DELAY);
    }
    motor_driver_->Stop();
    if (motor_mutex_ != nullptr) {
        xSemaphoreGive(motor_mutex_);
    }
    is_moving_ = false;
}

void WheelbotController::ActionTask(void* arg) {
    auto* controller = static_cast<WheelbotController*>(arg);
    ActionParams params;

    while (true) {
        if (xQueueReceive(controller->action_queue_, &params, pdMS_TO_TICKS(1000)) != pdTRUE) {
            continue;
        }

        controller->stop_requested_ = false;
        controller->is_moving_ = true;
        int left = 0, right = 0;
        switch (params.action_type) {
            case kActionForward:
                left = params.speed;
                right = params.speed;
                break;
            case kActionBackward:
                left = -params.speed;
                right = -params.speed;
                break;
            case kActionTurnLeft:
                left = -params.speed;
                right = params.speed;
                break;
            case kActionTurnRight:
                left = params.speed;
                right = -params.speed;
                break;
        }
        if (controller->motor_mutex_ != nullptr) {
            xSemaphoreTake(controller->motor_mutex_, portMAX_DELAY);
        }
        controller->motor_driver_->Drive(left, right);
        if (controller->motor_mutex_ != nullptr) {
            xSemaphoreGive(controller->motor_mutex_);
        }

        // Chunked wait so EmergencyStop() (setting stop_requested_) or a
        // newer command already sitting in the queue can interrupt a
        // long-running timed move instead of a single blocking delay.
        constexpr int kTickMs = 50;
        int waited = 0;
        while (waited < params.duration_ms && !controller->stop_requested_ &&
               uxQueueMessagesWaiting(controller->action_queue_) == 0) {
            vTaskDelay(pdMS_TO_TICKS(kTickMs));
            waited += kTickMs;
        }

        if (controller->motor_mutex_ != nullptr) {
            xSemaphoreTake(controller->motor_mutex_, portMAX_DELAY);
        }
        controller->motor_driver_->Stop();
        if (controller->motor_mutex_ != nullptr) {
            xSemaphoreGive(controller->motor_mutex_);
        }
        controller->is_moving_ = false;
    }
}

void WheelbotController::RegisterMcpTools() {
    auto& mcp = McpServer::GetInstance();

    auto move_properties = PropertyList({
        Property("duration_ms", kPropertyTypeInteger, 2000, 0, 30000),
        Property("speed", kPropertyTypeInteger, 80, 0, 100),
    });

    mcp.AddTool("self.wheelbot.move_forward", "Drive the robot forward for a duration.",
                move_properties, [this](const PropertyList& p) -> ReturnValue {
                    if (movement_inhibited_) {
                        return std::string(
                            "Movement blocked: anti-fall sensor reports unsafe (no floor "
                            "confirmed).");
                    }
                    if (!QueueMove(kActionForward, p["speed"].value<int>(),
                                   p["duration_ms"].value<int>())) {
                        return std::string("Failed to queue move (motor task unavailable).");
                    }
                    return true;
                });
    mcp.AddTool("self.wheelbot.move_backward", "Drive the robot backward for a duration.",
                move_properties, [this](const PropertyList& p) -> ReturnValue {
                    if (movement_inhibited_) {
                        return std::string(
                            "Movement blocked: anti-fall sensor reports unsafe (no floor "
                            "confirmed).");
                    }
                    if (!QueueMove(kActionBackward, p["speed"].value<int>(),
                                   p["duration_ms"].value<int>())) {
                        return std::string("Failed to queue move (motor task unavailable).");
                    }
                    return true;
                });
    mcp.AddTool("self.wheelbot.turn_left", "Turn the robot left in place for a duration.",
                move_properties, [this](const PropertyList& p) -> ReturnValue {
                    if (movement_inhibited_) {
                        return std::string(
                            "Movement blocked: anti-fall sensor reports unsafe (no floor "
                            "confirmed).");
                    }
                    if (!QueueMove(kActionTurnLeft, p["speed"].value<int>(),
                                   p["duration_ms"].value<int>())) {
                        return std::string("Failed to queue move (motor task unavailable).");
                    }
                    return true;
                });
    mcp.AddTool("self.wheelbot.turn_right", "Turn the robot right in place for a duration.",
                move_properties, [this](const PropertyList& p) -> ReturnValue {
                    if (movement_inhibited_) {
                        return std::string(
                            "Movement blocked: anti-fall sensor reports unsafe (no floor "
                            "confirmed).");
                    }
                    if (!QueueMove(kActionTurnRight, p["speed"].value<int>(),
                                   p["duration_ms"].value<int>())) {
                        return std::string("Failed to queue move (motor task unavailable).");
                    }
                    return true;
                });
    mcp.AddTool("self.wheelbot.stop", "Immediately stop all motor movement.", PropertyList(),
                [this](const PropertyList&) -> ReturnValue {
                    EmergencyStop();
                    return true;
                });

    mcp.AddTool("self.wheelbot.set_motor_type",
                "Switch the motor backend between the continuous-rotation servo pair and the L298N "
                "DC motor driver. Persists and reboots the device to apply.",
                PropertyList({Property("type", kPropertyTypeString)}),
                [this](const PropertyList& p) -> ReturnValue {
                    auto type_str = p["type"].value<std::string>();
                    MotorType type;
                    if (type_str == "l298n") {
                        type = MotorType::kL298n;
                    } else if (type_str == "servo") {
                        type = MotorType::kServo;
                    } else {
                        return std::string("type must be exactly \"l298n\" or \"servo\".");
                    }
                    SaveMotorTypeAndPins(type, settings_.in1, settings_.in2, settings_.in3,
                                         settings_.in4);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    Application::GetInstance().Reboot();
                    return true;
                });

    mcp.AddTool("self.wheelbot.set_motor_pins",
                "Remap the 4 L298N control GPIOs (in1-in4). Persists and reboots to apply.",
                PropertyList({
                    Property("in1", kPropertyTypeInteger, 0, 48),
                    Property("in2", kPropertyTypeInteger, 0, 48),
                    Property("in3", kPropertyTypeInteger, 0, 48),
                    Property("in4", kPropertyTypeInteger, 0, 48),
                }),
                [this](const PropertyList& p) -> ReturnValue {
                    auto in1 = static_cast<gpio_num_t>(p["in1"].value<int>());
                    auto in2 = static_cast<gpio_num_t>(p["in2"].value<int>());
                    auto in3 = static_cast<gpio_num_t>(p["in3"].value<int>());
                    auto in4 = static_cast<gpio_num_t>(p["in4"].value<int>());
                    if (const char* error = ValidateMotorPins(in1, in2, in3, in4)) {
                        return std::string(error);
                    }
                    SaveMotorTypeAndPins(settings_.type, in1, in2, in3, in4);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    Application::GetInstance().Reboot();
                    return true;
                });

    mcp.AddTool("self.wheelbot.get_motor_config", "Get the current motor backend and pin config.",
                PropertyList(), [this](const PropertyList&) -> ReturnValue {
                    cJSON* json = cJSON_CreateObject();
                    cJSON_AddStringToObject(
                        json, "type", settings_.type == MotorType::kL298n ? "l298n" : "servo");
                    cJSON_AddNumberToObject(json, "in1", settings_.in1);
                    cJSON_AddNumberToObject(json, "in2", settings_.in2);
                    cJSON_AddNumberToObject(json, "in3", settings_.in3);
                    cJSON_AddNumberToObject(json, "in4", settings_.in4);
                    cJSON_AddNumberToObject(json, "stop_pulse_us", settings_.stop_pulse_us);
                    cJSON_AddBoolToObject(json, "reverse_left", settings_.reverse_left);
                    cJSON_AddBoolToObject(json, "reverse_right", settings_.reverse_right);
                    return json;
                });

    mcp.AddTool(
        "self.wheelbot.get_control_status",
        "Report whether the robot is currently moving, and which transport (local_ws or "
        "cloud_voice) sent the current/most recent move command -- useful for a UI to show "
        "when another controller (e.g. the phone app vs. voice) has taken over movement.",
        PropertyList(), [this](const PropertyList&) -> ReturnValue {
            static const char* kActionNames[] = {"none", "forward", "backward", "turn_left",
                                                 "turn_right"};
            int action_type = active_action_type_.load();
            const char* action_name =
                (action_type >= 0 && action_type < 5) ? kActionNames[action_type] : "unknown";
            cJSON* json = cJSON_CreateObject();
            cJSON_AddBoolToObject(json, "is_moving", is_moving_);
            cJSON_AddStringToObject(json, "last_action", action_name);
            cJSON_AddStringToObject(json, "last_action_source",
                                    CommandSourceName(active_source_.load()));
            return json;
        });

    mcp.AddTool(
        "self.wheelbot.set_servo_stop_pulse",
        "Calibrate the continuous-rotation servo stop/neutral pulse width. Applies live, no "
        "reboot needed. Only affects the servo motor backend.",
        PropertyList({Property("microseconds", kPropertyTypeInteger, 1500, 1000, 2000)}),
        [this](const PropertyList& p) -> ReturnValue {
            if (servo_driver_ == nullptr) {
                return std::string("Current motor backend is not the servo pair.");
            }
            int us = p["microseconds"].value<int>();
            servo_driver_->SetStopPulseUs(us);
            settings_.stop_pulse_us = us;
            SaveServoStopPulseUs(us);
            return true;
        });

    mcp.AddTool(
        "self.wheelbot.set_servo_reverse",
        "Reverse one side's continuous-rotation servo direction. Applies live. Only affects "
        "the servo motor backend.",
        PropertyList({
            Property("side", kPropertyTypeString),
            Property("reversed", kPropertyTypeBoolean),
        }),
        [this](const PropertyList& p) -> ReturnValue {
            if (servo_driver_ == nullptr) {
                return std::string("Current motor backend is not the servo pair.");
            }
            auto side = p["side"].value<std::string>();
            if (side != "left" && side != "right") {
                return std::string("side must be exactly \"left\" or \"right\".");
            }
            bool left = side == "left";
            bool reversed = p["reversed"].value<bool>();
            servo_driver_->SetReverse(left, reversed);
            if (left) {
                settings_.reverse_left = reversed;
            } else {
                settings_.reverse_right = reversed;
            }
            SaveServoReverse(left, reversed);
            return true;
        });
}
