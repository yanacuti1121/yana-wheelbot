#include <driver/i2c_master.h>
#include <driver/spi_common.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_log.h>

#if CONFIG_YANA_WHEELBOT_DISPLAY_ST7735
#include <esp_lcd_st7735.h>
#endif

#include "application.h"
#include "arm_neck_controller.h"
#include "button.h"
#include "cliff_sensor_controller.h"
#include "codecs/no_audio_codec.h"
#include "config.h"
#include "display/lcd_display.h"
#include "display/lvgl_display/lvgl_theme.h"
#include "dual_led_controller.h"
#include "mcp_server.h"
#include "settings.h"
#include "tof_sensor.h"
#include "vl53l0x.h"
#include "vl6180x.h"
#include "websocket_control_server.h"
#include "wheelbot_controller.h"
#include "wifi_board.h"

#define TAG "YanaWheelbot"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);

namespace {
// self.screen.set_orientation is registered here (not in a dedicated
// controller) since it only needs to persist+reboot, mirroring
// DualNetworkBoard::SwitchNetworkType()'s pattern -- see
// main/boards/common/dual_network_board.cc.
void RegisterDisplayOrientationTool() {
    auto& mcp = McpServer::GetInstance();
    mcp.AddTool("self.screen.set_orientation",
                "Switch the display between portrait (128x160) and landscape. Persists and reboots "
                "to apply -- runtime panel re-init without a reboot is not verified safe on this "
                "board yet.",
                PropertyList({Property("orientation", kPropertyTypeString)}),
                [](const PropertyList& p) -> ReturnValue {
                    auto orientation = p["orientation"].value<std::string>();
                    if (orientation != "portrait" && orientation != "landscape") {
                        return std::string("orientation must be \"portrait\" or \"landscape\"");
                    }
                    Settings settings("display", true);
                    settings.SetString("orientation", orientation);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    Application::GetInstance().Reboot();
                    return true;
                });
}
}  // namespace

class YanaWheelbotBoard : public WifiBoard {
private:
    LcdDisplay* display_ = nullptr;
    Button boot_button_;
    Button touch_button_;
    AudioCodec* audio_codec_ = nullptr;
    i2c_master_bus_handle_t tof_i2c_bus_ = nullptr;
    TofSensor* tof_sensor_ = nullptr;
    WebSocketControlServer* ws_control_server_ = nullptr;

    WheelbotController* wheelbot_controller_ = nullptr;
    CliffSensorController* cliff_sensor_controller_ = nullptr;
    DualLedController* led_controller_ = nullptr;
    ArmNeckController* arm_neck_controller_ = nullptr;

    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = WHEELBOT_HARDWARE_CONFIG.display_mosi_pin;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = WHEELBOT_HARDWARE_CONFIG.display_clk_pin;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz =
            DISPLAY_PORTRAIT_WIDTH * DISPLAY_PORTRAIT_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    void InitializeLcdDisplay() {
        // Orientation is loaded once at boot and applied by swapping
        // width/height + swap_xy; changing it later goes through
        // self.screen.set_orientation, which persists + reboots rather than
        // attempting a live re-layout (see the tool's own description).
        Settings settings("display", false);
        bool landscape = settings.GetString("orientation", "portrait") == "landscape";
        int width = landscape ? DISPLAY_PORTRAIT_HEIGHT : DISPLAY_PORTRAIT_WIDTH;
        int height = landscape ? DISPLAY_PORTRAIT_WIDTH : DISPLAY_PORTRAIT_HEIGHT;

        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = WHEELBOT_HARDWARE_CONFIG.display_cs_pin;
        io_config.dc_gpio_num = WHEELBOT_HARDWARE_CONFIG.display_dc_pin;
        io_config.spi_mode = DISPLAY_SPI_MODE;
        io_config.pclk_hz = 40 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI3_HOST, &io_config, &panel_io));

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = WHEELBOT_HARDWARE_CONFIG.display_rst_pin;
        panel_config.rgb_ele_order = DISPLAY_RGB_ORDER;
        panel_config.bits_per_pixel = 16;

#if CONFIG_YANA_WHEELBOT_DISPLAY_ST7735
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7735(panel_io, &panel_config, &panel));
#else
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
#endif

        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, DISPLAY_INVERT_COLOR);
        esp_lcd_panel_swap_xy(panel, landscape);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);

        display_ =
            new SpiLcdDisplay(panel_io, panel, width, height, DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y,
                              DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, landscape);

        // "light"/"dark" are already registered by LcdDisplay's own
        // constructor (see main/display/lcd_display.cc's InitializeLcdThemes).
        // Add a 3rd named theme; self.screen.set_theme (registered generically
        // by mcp_server.cc's AddCommonTools()) picks it up with no further code.
        auto text_font = std::make_shared<LvglBuiltInFont>(&BUILTIN_TEXT_FONT);
        auto icon_font = std::make_shared<LvglBuiltInFont>(&BUILTIN_ICON_FONT);
        auto ocean_theme = new LvglTheme("ocean");
        ocean_theme->set_background_color(lv_color_hex(0x0B2A3D));
        ocean_theme->set_text_color(lv_color_hex(0xE6F6FF));
        ocean_theme->set_chat_background_color(lv_color_hex(0x123A52));
        ocean_theme->set_user_bubble_color(lv_color_hex(0x2FA6D9));
        ocean_theme->set_assistant_bubble_color(lv_color_hex(0x1B4E6B));
        ocean_theme->set_system_bubble_color(lv_color_hex(0x0B2A3D));
        ocean_theme->set_system_text_color(lv_color_hex(0xE6F6FF));
        ocean_theme->set_border_color(lv_color_hex(0x2FA6D9));
        ocean_theme->set_low_battery_color(lv_color_hex(0xFF6B6B));
        ocean_theme->set_text_font(text_font);
        ocean_theme->set_icon_font(icon_font);
        LvglThemeManager::GetInstance().RegisterTheme("ocean", ocean_theme);
    }

    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
        // TTP223 double-tap toggles chat, mirroring the boot button above and
        // the KST AI Robot's own double-tap-to-chat behavior on this same pin.
        touch_button_.OnDoubleClick(
            [this]() { Application::GetInstance().ToggleChatState(); });
    }

    void InitializeAudioCodec() {
        audio_codec_ = new NoAudioCodecSimplex(WHEELBOT_HARDWARE_CONFIG.audio_input_sample_rate,
                                               WHEELBOT_HARDWARE_CONFIG.audio_output_sample_rate,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_spk_gpio_bclk,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_spk_gpio_lrck,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_spk_gpio_dout,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_mic_gpio_sck,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_mic_gpio_ws,
                                               WHEELBOT_HARDWARE_CONFIG.audio_i2s_mic_gpio_din);
    }

    void InitializeTofSensor() {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = WHEELBOT_HARDWARE_CONFIG.tof_i2c_sda_pin,
            .scl_io_num = WHEELBOT_HARDWARE_CONFIG.tof_i2c_scl_pin,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {.enable_internal_pullup = 1},
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &tof_i2c_bus_));
#if CONFIG_YANA_WHEELBOT_TOF_VL6180X
        tof_sensor_ = new Vl6180x(tof_i2c_bus_, VL6180X_I2C_ADDR);
#else
        tof_sensor_ = new Vl53l0x(tof_i2c_bus_, VL53L0X_I2C_ADDR);
#endif
    }

    void InitializeWebSocketControlServer() {
        ws_control_server_ = new WebSocketControlServer();
        if (!ws_control_server_->Start(8080)) {
            delete ws_control_server_;
            ws_control_server_ = nullptr;
            return;
        }
        Application::GetInstance().RegisterMcpBroadcastCallback([this](const std::string& payload) {
            if (ws_control_server_) {
                ws_control_server_->BroadcastMessage(payload);
            }
        });
        // wheelbot_controller_ is always constructed first, in the board's
        // own constructor -- StartNetwork() (which calls this method) only
        // runs afterward, so this is never null here.
        ws_control_server_->SetOnAllClientsDisconnected(
            [this]() { wheelbot_controller_->EmergencyStop(); });
    }

    void StartNetwork() override {
        WifiBoard::StartNetwork();
        vTaskDelay(pdMS_TO_TICKS(1000));
        InitializeWebSocketControlServer();
    }

public:
    YanaWheelbotBoard()
        : boot_button_(BOOT_BUTTON_GPIO),
          touch_button_(WHEELBOT_HARDWARE_CONFIG.touch_sensor_pin, /*active_high=*/true) {
        InitializeSpi();
        InitializeLcdDisplay();
        InitializeButtons();
        InitializeAudioCodec();
        InitializeTofSensor();

        wheelbot_controller_ = new WheelbotController(
            WHEELBOT_HARDWARE_CONFIG.motor_in1_pin, WHEELBOT_HARDWARE_CONFIG.motor_in2_pin,
            WHEELBOT_HARDWARE_CONFIG.motor_in3_pin, WHEELBOT_HARDWARE_CONFIG.motor_in4_pin);
        cliff_sensor_controller_ = new CliffSensorController(tof_sensor_, wheelbot_controller_);
        led_controller_ = new DualLedController(WHEELBOT_HARDWARE_CONFIG.led_left_pin,
                                                WHEELBOT_HARDWARE_CONFIG.led_right_pin);
        arm_neck_controller_ = new ArmNeckController(WHEELBOT_HARDWARE_CONFIG.arm_servo_pin,
                                                     WHEELBOT_HARDWARE_CONFIG.neck_servo_pin);
        RegisterDisplayOrientationTool();
        GetBacklight()->RestoreBrightness();
    }

    virtual AudioCodec* GetAudioCodec() override { return audio_codec_; }
    virtual Display* GetDisplay() override { return display_; }
    virtual Led* GetLed() override { return led_controller_; }

    virtual Backlight* GetBacklight() override {
        static PwmBacklight* backlight = nullptr;
        if (backlight == nullptr) {
            backlight = new PwmBacklight(WHEELBOT_HARDWARE_CONFIG.display_backlight_pin,
                                         DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        }
        return backlight;
    }
};

DECLARE_BOARD(YanaWheelbotBoard);
