#ifndef CLIFF_SENSOR_CONTROLLER_H
#define CLIFF_SENSOR_CONTROLLER_H

#include <atomic>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "tof_sensor.h"
#include "wheelbot_controller.h"

// Polls the ToF sensor (VL53L0X or VL6180X, selected at build time -- see
// yana_wheelbot_board.cc) for anti-fall/cliff detection while enabled, and
// forces an emergency stop via WheelbotController when no floor is detected
// within the configured safe-distance threshold. Settings namespace:
// "cliff_sensor" (keys: "enabled" bool, "threshold_mm" int).
//
// Sensor is assumed downward-facing (per every "anti-fall" reference in this
// board's docs): a small reading means the floor is close (safe), a large
// reading -- or a sensor error/timeout (negative return from
// TofSensor::ReadDistanceMm()) -- means no floor was found within range,
// i.e. a cliff/edge, and must fail safe (stop), not fail open. Requires
// kRequiredConsecutiveUnsafeReads consecutive unsafe polls before actually
// stopping, to absorb single-sample sensor noise without meaningfully
// delaying a real stop (poll interval is short, see kPollIntervalMs in the
// .cc file).
//
// EmergencyStop() alone only interrupts the move that's already running --
// it does not stop WheelbotController from accepting the *next* QueueMove()
// call. Whatever is sending moves (the web controller's heartbeat pulse,
// voice) has no idea the floor is unsafe and keeps sending, so a bare
// EmergencyStop() produces a stop/restart stutter right at the edge instead
// of an actual hold. To close that gap, this controller also latches
// WheelbotController::SetMovementInhibited(true) once unsafe is confirmed,
// and only clears it after kRequiredConsecutiveSafeReads consecutive safe
// polls -- so every movement tool call is rejected for the whole time the
// sensor reports unsafe, not just the instant it was detected.
class CliffSensorController {
public:
    CliffSensorController(TofSensor* sensor, WheelbotController* wheelbot_controller);
    ~CliffSensorController();

private:
    void RegisterMcpTools();
    static void PollTask(void* arg);

    TofSensor* sensor_;
    WheelbotController* wheelbot_controller_;
    std::atomic<bool> enabled_{true};
    std::atomic<int> threshold_mm_{50};
    TaskHandle_t poll_task_handle_ = nullptr;
    int consecutive_unsafe_reads_ = 0;
    int consecutive_safe_reads_ = 0;

    static constexpr int kRequiredConsecutiveUnsafeReads = 2;
    static constexpr int kRequiredConsecutiveSafeReads = 3;
};

#endif  // CLIFF_SENSOR_CONTROLLER_H
