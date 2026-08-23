#ifndef WHEELBOT_CONTROLLER_H
#define WHEELBOT_CONTROLLER_H

#include <atomic>
#include <memory>

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "command_source.h"
#include "motor_driver.h"
#include "motor_settings.h"
#include "servo_motor_driver.h"

// Owns the active MotorDriver and registers every movement / motor-config /
// servo-calibration MCP tool, following main/boards/otto-robot/otto_controller.cc's
// pattern: a dedicated FreeRTOS action task + queue so MCP callbacks return
// immediately instead of blocking on motor timing.
class WheelbotController {
public:
    WheelbotController(gpio_num_t default_in1, gpio_num_t default_in2, gpio_num_t default_in3,
                       gpio_num_t default_in4);
    ~WheelbotController();

    // Called by CliffSensorController (or anything else) to force an
    // immediate stop, interrupting any in-flight timed move.
    void EmergencyStop();

    // Anti-fall latch: while inhibited, QueueMove() rejects every new move
    // instead of accepting it. EmergencyStop() alone only interrupts the
    // move already running -- it says nothing about the next QueueMove()
    // call, so a client that keeps sending moves (e.g. the web controller's
    // heartbeat pulse) would otherwise start the robot moving again on the
    // very next command. CliffSensorController sets this once the ToF
    // sensor confirms unsafe, and clears it only after confirming several
    // consecutive safe reads -- see cliff_sensor_controller.h.
    void SetMovementInhibited(bool inhibited) { movement_inhibited_ = inhibited; }
    bool IsMovementInhibited() const { return movement_inhibited_; }

private:
    enum ActionType {
        kActionForward = 1,
        kActionBackward = 2,
        kActionTurnLeft = 3,
        kActionTurnRight = 4,
    };

    struct ActionParams {
        int action_type;
        int speed;
        int duration_ms;
        CommandSource source;
    };

    void BuildMotorDriver();
    void RegisterMcpTools();
    // Returns false if the action queue doesn't exist (construction failed)
    // or the move couldn't be enqueued -- callers must surface this as an
    // error, not claim success.
    bool QueueMove(ActionType action, int speed, int duration_ms);
    // Returns nullptr if in1-in4 are all distinct, valid, non-reserved
    // output-capable GPIOs; otherwise a human-readable reason they aren't.
    // See the .cc file for the exact reserved-pin list and why.
    static const char* ValidateMotorPins(gpio_num_t in1, gpio_num_t in2, gpio_num_t in3,
                                         gpio_num_t in4);

    static void ActionTask(void* arg);

    MotorSettingsData settings_;
    std::unique_ptr<MotorDriver> motor_driver_;
    ServoMotorDriver* servo_driver_ = nullptr;  // non-owning alias, only valid when type==kServo

    QueueHandle_t action_queue_ = nullptr;
    TaskHandle_t action_task_handle_ = nullptr;
    // Guards every motor_driver_->Drive()/Stop() call -- ActionTask and
    // EmergencyStop() (called from CliffSensorController's own task, or an
    // MCP handler) can both reach the driver concurrently otherwise.
    SemaphoreHandle_t motor_mutex_ = nullptr;
    std::atomic<bool> stop_requested_{false};
    std::atomic<bool> is_moving_{false};
    std::atomic<bool> movement_inhibited_{false};
    // Who queued/is running the current or most recent action -- see
    // command_source.h and self.wheelbot.get_control_status. Only
    // meaningful for move/turn/stop commands going through QueueMove()
    // /EmergencyStop(); other tools (set_motor_type etc.) don't update this.
    std::atomic<CommandSource> active_source_{CommandSource::kCloudVoice};
    std::atomic<int> active_action_type_{0};  // 0 = none yet
};

#endif  // WHEELBOT_CONTROLLER_H
