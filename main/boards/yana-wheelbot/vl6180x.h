#ifndef VL6180X_H
#define VL6180X_H

#include <driver/i2c_master.h>

#include "tof_sensor.h"

// Minimal VL6180X time-of-flight distance sensor driver (alternate to
// VL53L0X, selected via Kconfig YANA_WHEELBOT_TOF_VL6180X). Shorter range
// (~200mm max) than VL53L0X (~2m), but a common substitute (e.g. the KST AI
// Robot's public wiring diagram lists VL6180X/TOF050C as its anti-fall
// sensor -- see this board's README "Credit" section).
//
// Does NOT inherit I2cDevice: VL6180X uses 16-bit register addresses,
// unlike VL53L0X's 8-bit ones that I2cDevice's WriteReg/ReadReg assume, so
// this class does its own minimal I2C read/write via the same underlying
// i2c_master_transmit/i2c_master_transmit_receive APIs I2cDevice itself
// uses internally (main/boards/common/i2c_device.cc).
//
// SCOPE NOTE (same honest-scope reasoning as vl53l0x.h): this driver does
// presence detection (model ID register), ST's mandatory one-time SR03
// "private register" init sequence from AN4545 section 9 (run once, gated
// by the SYSTEM_FRESH_OUT_OF_RESET flag, same as every maintained VL6180X
// driver does), and the basic single-shot ranging trigger/poll/read
// sequence. It does NOT implement AN4545's separate "recommended" tuning
// (readout averaging period, ALS gain/integration, interrupt config, VHV
// repeat rate) -- those affect measurement quality/repeatability, not
// whether ranging works at all, so they're left out to keep this driver's
// scope to "produces a usable reading", matching vl53l0x.h's own scope.
// The private-register table itself is cross-verified against two
// independent MIT-licensed drivers (github.com/pololu/vl6180x-arduino,
// github.com/adafruit/Adafruit_VL6180X) implementing the same AN4545
// section -- not copied from either one's code, just their register
// values, which are ST's public datasheet numbers, not anyone's original
// expression. Verify readings against a known distance on real hardware
// before trusting them for anti-fall safety.
class Vl6180x : public TofSensor {
public:
    Vl6180x(i2c_master_bus_handle_t i2c_bus, uint8_t addr = 0x29);

    // Returns true if the model-ID register matches the known VL6180X value.
    bool Probe();

    int ReadDistanceMm() override;

private:
    void WriteReg8(uint16_t reg, uint8_t value);
    uint8_t ReadReg8(uint16_t reg);
    // Runs the AN4545 mandatory SR03 private-register sequence once, only
    // if SYSTEM_FRESH_OUT_OF_RESET reads 1 (i.e. not yet initialized since
    // power-on/reset), then clears that flag.
    void InitMandatoryRegistersIfNeeded();

    i2c_master_dev_handle_t device_;

    static constexpr uint16_t kRegModelId = 0x0000;
    static constexpr uint8_t kExpectedModelId = 0xB4;
    static constexpr uint16_t kRegSysRangeStart = 0x0018;
    static constexpr uint16_t kRegResultInterruptStatusGpio = 0x004F;
    static constexpr uint16_t kRegResultRangeVal = 0x0062;
    static constexpr uint16_t kRegSystemInterruptClear = 0x0015;
    static constexpr uint16_t kRegSystemFreshOutOfReset = 0x0016;
};

#endif  // VL6180X_H
