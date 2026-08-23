#include "vl53l0x.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "Vl53l0x";

Vl53l0x::Vl53l0x(i2c_master_bus_handle_t i2c_bus, uint8_t addr) : I2cDevice(i2c_bus, addr) {}

bool Vl53l0x::Probe() {
    // NOTE: I2cDevice::ReadReg() uses ESP_ERROR_CHECK internally and will abort
    // the firmware on an I2C transaction failure (e.g. sensor not physically
    // wired) rather than returning an error code — this matches every other
    // I2cDevice subclass in this codebase, which all assume their peripheral is
    // present. Only call Probe()/ReadDistanceMm() when the cliff sensor feature
    // is actually enabled by the user, not unconditionally at boot.
    uint8_t model_id = ReadReg(kRegModelId);
    if (model_id != kExpectedModelId) {
        ESP_LOGW(TAG, "Unexpected model ID 0x%02X (expected 0x%02X)", model_id, kExpectedModelId);
        return false;
    }
    return true;
}

int Vl53l0x::ReadDistanceMm() {
    WriteReg(kRegSysRangeStart, 0x01);

    constexpr int kMaxWaitMs = 100;
    constexpr int kPollIntervalMs = 5;
    int waited = 0;
    while (waited < kMaxWaitMs) {
        uint8_t status = ReadReg(kRegResultInterruptStatus);
        if ((status & 0x07) != 0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(kPollIntervalMs));
        waited += kPollIntervalMs;
    }
    if (waited >= kMaxWaitMs) {
        ESP_LOGW(TAG, "Ranging measurement timed out");
        return -1;
    }

    uint8_t buffer[12];
    ReadRegs(kRegResultRangeStatus, buffer, sizeof(buffer));
    int mm = (buffer[kRangeMmOffset] << 8) | buffer[kRangeMmOffset + 1];

    WriteReg(kRegSystemInterruptClear, 0x01);
    return mm;
}
