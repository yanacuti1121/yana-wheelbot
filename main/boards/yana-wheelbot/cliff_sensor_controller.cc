#include "cliff_sensor_controller.h"

#include <esp_log.h>
#include <cJSON.h>

#include "mcp_server.h"
#include "settings.h"

static const char* TAG = "CliffSensorController";
static constexpr const char* kNamespace = "cliff_sensor";
static constexpr int kPollIntervalMs = 200;

CliffSensorController::CliffSensorController(TofSensor* sensor,
                                             WheelbotController* wheelbot_controller)
    : sensor_(sensor), wheelbot_controller_(wheelbot_controller) {
    Settings settings(kNamespace, true);
    enabled_ = settings.GetBool("enabled", true);
    threshold_mm_ = settings.GetInt("threshold_mm", 50);

    xTaskCreate(PollTask, "CliffSensorPoll", 3072, this, tskIDLE_PRIORITY + 1, &poll_task_handle_);

    RegisterMcpTools();
}

CliffSensorController::~CliffSensorController() {
    if (poll_task_handle_ != nullptr) {
        vTaskDelete(poll_task_handle_);
    }
}

void CliffSensorController::PollTask(void* arg) {
    auto* controller = static_cast<CliffSensorController*>(arg);

    while (true) {
        if (controller->enabled_) {
            int mm = controller->sensor_->ReadDistanceMm();
            // Fail-safe, not fail-open: a sensor error/timeout (negative)
            // means "no floor confirmed", same as a reading past the
            // threshold -- both count as unsafe. See this file's header
            // comment for why the polarity is ">= threshold", not "<".
            bool unsafe = mm < 0 || mm >= controller->threshold_mm_;
            if (unsafe) {
                controller->consecutive_unsafe_reads_++;
                controller->consecutive_safe_reads_ = 0;
            } else {
                controller->consecutive_unsafe_reads_ = 0;
                controller->consecutive_safe_reads_++;
            }
            if (controller->consecutive_unsafe_reads_ >= kRequiredConsecutiveUnsafeReads) {
                if (!controller->wheelbot_controller_->IsMovementInhibited()) {
                    ESP_LOGW(TAG,
                             "Cliff detected (last reading %d mm, threshold %d mm), emergency stop "
                             "+ movement inhibited",
                             mm, controller->threshold_mm_.load());
                }
                controller->wheelbot_controller_->EmergencyStop();
                controller->wheelbot_controller_->SetMovementInhibited(true);
            } else if (controller->wheelbot_controller_->IsMovementInhibited() &&
                       controller->consecutive_safe_reads_ >= kRequiredConsecutiveSafeReads) {
                ESP_LOGI(TAG, "Floor confirmed safe (%d consecutive reads), movement re-enabled",
                         controller->consecutive_safe_reads_);
                controller->wheelbot_controller_->SetMovementInhibited(false);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(kPollIntervalMs));
    }
}

void CliffSensorController::RegisterMcpTools() {
    auto& mcp = McpServer::GetInstance();

    mcp.AddTool("self.cliff_sensor.set_enabled", "Enable or disable the anti-fall ToF sensor.",
                PropertyList({Property("enabled", kPropertyTypeBoolean)}),
                [this](const PropertyList& p) -> ReturnValue {
                    bool enabled = p["enabled"].value<bool>();
                    enabled_ = enabled;
                    if (!enabled) {
                        // Disabling the sensor must not leave movement
                        // stuck inhibited forever -- PollTask only clears
                        // the latch while enabled_ is true, so it can never
                        // self-recover once the sensor is off.
                        consecutive_unsafe_reads_ = 0;
                        consecutive_safe_reads_ = 0;
                        wheelbot_controller_->SetMovementInhibited(false);
                    }
                    Settings settings(kNamespace, true);
                    settings.SetBool("enabled", enabled);
                    return true;
                });

    mcp.AddTool("self.cliff_sensor.set_threshold",
                "Set the safe-distance threshold (mm): the robot emergency-stops if the downward "
                "ToF sensor reads at or above this distance (no floor confirmed) or reports an "
                "error.",
                PropertyList({Property("threshold_mm", kPropertyTypeInteger, 50, 5, 500)}),
                [this](const PropertyList& p) -> ReturnValue {
                    int threshold = p["threshold_mm"].value<int>();
                    threshold_mm_ = threshold;
                    Settings settings(kNamespace, true);
                    settings.SetInt("threshold_mm", threshold);
                    return true;
                });

    mcp.AddTool("self.cliff_sensor.get_config", "Get the current anti-fall sensor configuration.",
                PropertyList(), [this](const PropertyList&) -> ReturnValue {
                    cJSON* json = cJSON_CreateObject();
                    cJSON_AddBoolToObject(json, "enabled", enabled_);
                    cJSON_AddNumberToObject(json, "threshold_mm", threshold_mm_);
                    return json;
                });

    mcp.AddTool("self.cliff_sensor.test_now",
                "Trigger a single distance reading right now and return the value in mm.",
                PropertyList(),
                [this](const PropertyList&) -> ReturnValue { return sensor_->ReadDistanceMm(); });
}
