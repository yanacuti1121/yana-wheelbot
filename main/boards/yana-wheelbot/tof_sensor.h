#ifndef TOF_SENSOR_H
#define TOF_SENSOR_H

// Common interface for the two selectable anti-fall ToF sensor drivers
// (VL53L0X, VL6180X), so CliffSensorController doesn't need to know which
// concrete chip is wired up. Selected at build time via Kconfig
// (YANA_WHEELBOT_TOF_VL6180X) -- see yana_wheelbot_board.cc.
class TofSensor {
public:
    virtual ~TofSensor() = default;

    // Triggers a single-shot ranging measurement and returns the result in
    // millimeters, or -1 on timeout/error.
    virtual int ReadDistanceMm() = 0;
};

#endif  // TOF_SENSOR_H
