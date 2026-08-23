#include "vl6180x.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* TAG = "Vl6180x";

Vl6180x::Vl6180x(i2c_master_bus_handle_t i2c_bus, uint8_t addr) {
    i2c_device_config_t cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 400 * 1000,
        .scl_wait_us = 0,
        .flags = {.disable_ack_check = 0},
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &cfg, &device_));
    InitMandatoryRegistersIfNeeded();
}

void Vl6180x::InitMandatoryRegistersIfNeeded() {
    if (ReadReg8(kRegSystemFreshOutOfReset) != 1) {
        return;  // Already initialized since last power-on/reset.
    }

    // AN4545 section 9, "Mandatory : private registers" (SR03 settings).
    // Register/value pairs are ST's public datasheet numbers, cross-checked
    // against pololu/vl6180x-arduino and Adafruit_VL6180X (both MIT).
    static constexpr struct {
        uint16_t reg;
        uint8_t value;
    } kMandatoryRegisters[] = {
        {0x0207, 0x01}, {0x0208, 0x01}, {0x0096, 0x00}, {0x0097, 0xFD},
        {0x00E3, 0x01}, {0x00E4, 0x03}, {0x00E5, 0x02}, {0x00E6, 0x01},
        {0x00E7, 0x03}, {0x00F5, 0x02}, {0x00D9, 0x05}, {0x00DB, 0xCE},
        {0x00DC, 0x03}, {0x00DD, 0xF8}, {0x009F, 0x00}, {0x00A3, 0x3C},
        {0x00B7, 0x00}, {0x00BB, 0x3C}, {0x00B2, 0x09}, {0x00CA, 0x09},
        {0x0198, 0x01}, {0x01B0, 0x17}, {0x01AD, 0x00}, {0x00FF, 0x05},
        {0x0100, 0x05}, {0x0199, 0x05}, {0x01A6, 0x1B}, {0x01AC, 0x3E},
        {0x01A7, 0x1F}, {0x0030, 0x00},
    };
    for (const auto& r : kMandatoryRegisters) {
        WriteReg8(r.reg, r.value);
    }

    WriteReg8(kRegSystemFreshOutOfReset, 0x00);
    ESP_LOGI(TAG, "VL6180X mandatory SR03 init sequence applied");
}

void Vl6180x::WriteReg8(uint16_t reg, uint8_t value) {
    uint8_t buffer[3] = {static_cast<uint8_t>(reg >> 8), static_cast<uint8_t>(reg & 0xFF), value};
    ESP_ERROR_CHECK(i2c_master_transmit(device_, buffer, sizeof(buffer), 100));
}

uint8_t Vl6180x::ReadReg8(uint16_t reg) {
    uint8_t reg_buf[2] = {static_cast<uint8_t>(reg >> 8), static_cast<uint8_t>(reg & 0xFF)};
    uint8_t value = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(device_, reg_buf, sizeof(reg_buf), &value, 1, 100));
    return value;
}

bool Vl6180x::Probe() {
    // See this file's header comment: no I2cDevice base class here (16-bit
    // register addressing), and the mandatory ST private-register init
    // sequence is not implemented -- Probe()/ReadDistanceMm() should only be
    // called when the cliff sensor feature is actually enabled, same caution
    // as vl53l0x.h.
    uint8_t model_id = ReadReg8(kRegModelId);
    if (model_id != kExpectedModelId) {
        ESP_LOGW(TAG, "Unexpected model ID 0x%02X (expected 0x%02X)", model_id, kExpectedModelId);
        return false;
    }
    return true;
}

int Vl6180x::ReadDistanceMm() {
    WriteReg8(kRegSysRangeStart, 0x01);

    constexpr int kMaxWaitMs = 100;
    constexpr int kPollIntervalMs = 5;
    int waited = 0;
    while (waited < kMaxWaitMs) {
        uint8_t status = ReadReg8(kRegResultInterruptStatusGpio);
        if ((status & 0x04) != 0) {  // bit 2: a range result is ready
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(kPollIntervalMs));
        waited += kPollIntervalMs;
    }
    if (waited >= kMaxWaitMs) {
        ESP_LOGW(TAG, "Ranging measurement timed out");
        return -1;
    }

    // VL6180X's range result is a single byte (max ~200mm fits in uint8_t),
    // unlike VL53L0X's 16-bit value.
    int mm = ReadReg8(kRegResultRangeVal);

    WriteReg8(kRegSystemInterruptClear, 0x07);
    return mm;
}
