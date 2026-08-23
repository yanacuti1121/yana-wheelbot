#ifndef VL53L0X_H
#define VL53L0X_H

#include "i2c_device.h"
#include "tof_sensor.h"

// Minimal VL53L0X time-of-flight distance sensor driver, following
// main/boards/common/i2c_device.h's I2cDevice pattern (same base class used by
// main/boards/common/axp2101.h).
//
// SCOPE NOTE (flagged in the implementation plan, not an oversight): this
// driver only does presence detection (model ID register check) and the
// documented single-shot ranging trigger/poll/read sequence. It deliberately
// does NOT implement ST's full reference calibration (SPAD count/type,
// timing-budget tuning, signal-rate-limit calibration from
// VL53L0X_DataInit/StaticInit/PerformRefCalibration) — no VL53L0X code existed
// anywhere in this repo before, and reproducing that calibration sequence
// exactly without the datasheet/reference driver in hand risks silently wrong
// numbers presented as correct. Readings should be verified against a known
// distance on real hardware before being trusted; if accuracy is insufficient,
// replace this with ST's official API or a maintained ESP-IDF component.
class Vl53l0x : public I2cDevice, public TofSensor {
public:
    Vl53l0x(i2c_master_bus_handle_t i2c_bus, uint8_t addr = 0x29);

    // Returns true if the model-ID register matches the known VL53L0X value.
    bool Probe();

    int ReadDistanceMm() override;

private:
    static constexpr uint8_t kRegModelId = 0xC0;
    static constexpr uint8_t kExpectedModelId = 0xEE;
    static constexpr uint8_t kRegSysRangeStart = 0x00;
    static constexpr uint8_t kRegResultInterruptStatus = 0x13;
    static constexpr uint8_t kRegResultRangeStatus = 0x14;
    static constexpr uint8_t kRegSystemInterruptClear = 0x0B;
    static constexpr int kRangeMmOffset = 10;  // mm value is 16-bit big-endian at +10/+11
};

#endif  // VL53L0X_H
