#include "arm_neck_controller.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "mcp_server.h"

static const char* TAG = "ArmNeckController";

namespace {
constexpr int kArmHome = 90;
constexpr int kNeckHome = 90;
constexpr int kNeckLeft = 30;
constexpr int kNeckRight = 150;

// Short-lived task for the canned "wave" sequence, so the MCP callback that
// triggers it returns immediately (same reasoning as
// main/boards/otto-robot/otto_controller.cc's dedicated action task, just
// scoped to a single one-shot gesture instead of a persistent queue).
struct WaveTaskArgs {
    Oscillator* arm;
    SemaphoreHandle_t mutex;
};

void WaveTask(void* arg) {
    auto* args = static_cast<WaveTaskArgs*>(arg);
    for (int i = 0; i < 3; i++) {
        if (args->mutex != nullptr) xSemaphoreTake(args->mutex, portMAX_DELAY);
        args->arm->SetPosition(60);
        if (args->mutex != nullptr) xSemaphoreGive(args->mutex);
        vTaskDelay(pdMS_TO_TICKS(250));

        if (args->mutex != nullptr) xSemaphoreTake(args->mutex, portMAX_DELAY);
        args->arm->SetPosition(120);
        if (args->mutex != nullptr) xSemaphoreGive(args->mutex);
        vTaskDelay(pdMS_TO_TICKS(250));
    }
    if (args->mutex != nullptr) xSemaphoreTake(args->mutex, portMAX_DELAY);
    args->arm->SetPosition(kArmHome);
    if (args->mutex != nullptr) xSemaphoreGive(args->mutex);
    delete args;
    vTaskDelete(nullptr);
}
}  // namespace

ArmNeckController::ArmNeckController(gpio_num_t arm_pin, gpio_num_t neck_pin)
    : arm_pin_(arm_pin), neck_pin_(neck_pin) {
    arm_mutex_ = xSemaphoreCreateMutex();
    neck_mutex_ = xSemaphoreCreateMutex();
    if (arm_mutex_ == nullptr || neck_mutex_ == nullptr) {
        ESP_LOGE(TAG, "Failed to create arm/neck mutex -- concurrent tool calls are unguarded");
    }

    arm_.Attach(arm_pin_);
    arm_attached_ = true;
    arm_.SetPosition(kArmHome);
    neck_.Attach(neck_pin_);
    neck_attached_ = true;
    neck_.SetPosition(kNeckHome);

    RegisterMcpTools();
}

ArmNeckController::~ArmNeckController() {
    if (arm_mutex_ != nullptr) vSemaphoreDelete(arm_mutex_);
    if (neck_mutex_ != nullptr) vSemaphoreDelete(neck_mutex_);
}

void ArmNeckController::SetArmAngle(int angle) {
    if (arm_mutex_ != nullptr) xSemaphoreTake(arm_mutex_, portMAX_DELAY);
    arm_.SetPosition(angle);
    if (arm_mutex_ != nullptr) xSemaphoreGive(arm_mutex_);
}

void ArmNeckController::SetNeckAngle(int angle) {
    if (neck_mutex_ != nullptr) xSemaphoreTake(neck_mutex_, portMAX_DELAY);
    neck_.SetPosition(angle);
    if (neck_mutex_ != nullptr) xSemaphoreGive(neck_mutex_);
}

void ArmNeckController::Wave() {
    auto* args = new WaveTaskArgs{&arm_, arm_mutex_};
    if (xTaskCreate(WaveTask, "ArmWave", 2048, args, tskIDLE_PRIORITY + 1, nullptr) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create wave task (out of heap?) -- dropping this wave");
        delete args;
    }
}

void ArmNeckController::TurnNeck(const std::string& direction) {
    if (neck_mutex_ != nullptr) xSemaphoreTake(neck_mutex_, portMAX_DELAY);
    if (direction == "left") {
        neck_.SetPosition(kNeckLeft);
    } else if (direction == "right") {
        neck_.SetPosition(kNeckRight);
    } else {
        neck_.SetPosition(kNeckHome);
    }
    if (neck_mutex_ != nullptr) xSemaphoreGive(neck_mutex_);
}

void ArmNeckController::ReleaseArm() {
    if (arm_mutex_ != nullptr) xSemaphoreTake(arm_mutex_, portMAX_DELAY);
    arm_.Detach();
    arm_attached_ = false;
    if (arm_mutex_ != nullptr) xSemaphoreGive(arm_mutex_);
}

void ArmNeckController::ReleaseNeck() {
    if (neck_mutex_ != nullptr) xSemaphoreTake(neck_mutex_, portMAX_DELAY);
    neck_.Detach();
    neck_attached_ = false;
    if (neck_mutex_ != nullptr) xSemaphoreGive(neck_mutex_);
}

void ArmNeckController::RegisterMcpTools() {
    auto& mcp = McpServer::GetInstance();

    mcp.AddTool("self.arm.set_angle", "Set the arm servo angle (0-180 degrees).",
                PropertyList({Property("angle", kPropertyTypeInteger, 90, 0, 180)}),
                [this](const PropertyList& p) -> ReturnValue {
                    if (!arm_attached_) {
                        arm_.Attach(arm_pin_);
                        arm_attached_ = true;
                    }
                    SetArmAngle(p["angle"].value<int>());
                    return true;
                });

    mcp.AddTool("self.neck.set_angle", "Set the neck servo angle (0-180 degrees).",
                PropertyList({Property("angle", kPropertyTypeInteger, 90, 0, 180)}),
                [this](const PropertyList& p) -> ReturnValue {
                    if (!neck_attached_) {
                        neck_.Attach(neck_pin_);
                        neck_attached_ = true;
                    }
                    SetNeckAngle(p["angle"].value<int>());
                    return true;
                });

    mcp.AddTool("self.arm.wave", "Wave the arm back and forth a few times.", PropertyList(),
                [this](const PropertyList&) -> ReturnValue {
                    if (!arm_attached_) {
                        arm_.Attach(arm_pin_);
                        arm_attached_ = true;
                    }
                    Wave();
                    return true;
                });

    mcp.AddTool("self.neck.turn", "Turn the neck: left, right, or center.",
                PropertyList({Property("direction", kPropertyTypeString)}),
                [this](const PropertyList& p) -> ReturnValue {
                    if (!neck_attached_) {
                        neck_.Attach(neck_pin_);
                        neck_attached_ = true;
                    }
                    TurnNeck(p["direction"].value<std::string>());
                    return true;
                });

    mcp.AddTool("self.arm.release", "Stop driving the arm servo so it goes limp.", PropertyList(),
                [this](const PropertyList&) -> ReturnValue {
                    ReleaseArm();
                    return true;
                });

    mcp.AddTool("self.neck.release", "Stop driving the neck servo so it goes limp.", PropertyList(),
                [this](const PropertyList&) -> ReturnValue {
                    ReleaseNeck();
                    return true;
                });
}
