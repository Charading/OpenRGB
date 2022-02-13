/*-----------------------------------------*\
|  RazerController.h                        |
|                                           |
|  Definitions and types for Razer devices  |
|                                           |
|  Adam Honse (CalcProgrammer1) 1/22/2021   |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

/*---------------------------------------------------------*\
| Struct packing macro for GCC and MSVC                     |
\*---------------------------------------------------------*/
#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

/*---------------------------------------------------------*\
| Razer Device Mode IDs                                     |
\*---------------------------------------------------------*/
enum
{
    RAZER_DEVICE_MODE_HARDWARE                  = 0x00,
    RAZER_DEVICE_MODE_SOFTWARE                  = 0x03,
};

/*---------------------------------------------------------*\
| Razer Command IDs                                         |
\*---------------------------------------------------------*/
enum
{
    /*-----------------------------------------------------*\
    | Set Commands                                          |
    \*-----------------------------------------------------*/
    RAZER_COMMAND_ID_SET_LED_STATE              = 0x00,
    RAZER_COMMAND_ID_SET_DEVICE_MODE            = 0x04,

    /*-----------------------------------------------------*\
    | Get Commands                                          |
    \*-----------------------------------------------------*/
    RAZER_COMMAND_ID_GET_LED_STATE              = 0x80,
    RAZER_COMMAND_ID_GET_FIRMWARE_VERSION       = 0x81,
    RAZER_COMMAND_ID_GET_SERIAL_STRING          = 0x82,
    RAZER_COMMAND_ID_GET_DEVICE_MODE            = 0x84,
    RAZER_COMMAND_ID_GET_KEYBOARD_INFO          = 0x86,
};

/*---------------------------------------------------------*\
| Razer Storage Flags                                       |
\*---------------------------------------------------------*/
enum
{
    RAZER_STORAGE_NO_SAVE                       = 0x00,
    RAZER_STORAGE_SAVE                          = 0x01,
};

/*---------------------------------------------------------*\
| Razer LED IDs                                             |
\*---------------------------------------------------------*/
enum
{
    RAZER_LED_ID_ZERO                           = 0x00,
    RAZER_LED_ID_SCROLL_WHEEL                   = 0x01,
    RAZER_LED_ID_BATTERY                        = 0x03,
    RAZER_LED_ID_LOGO                           = 0x04,
    RAZER_LED_ID_BACKLIGHT                      = 0x05,
    RAZER_LED_ID_MACRO                          = 0x07,
    RAZER_LED_ID_GAME                           = 0x08,
    RAZER_LED_ID_PROFILE_RED                    = 0x0C,
    RAZER_LED_ID_PROFILE_GREEN                  = 0x0D,
    RAZER_LED_ID_PROFILE_BLUE                   = 0x0E,
    RAZER_LED_ID_RIGHT_SIDE                     = 0x10,
    RAZER_LED_ID_LEFT_SIDE                      = 0x11,
    RAZER_LED_ID_ARGB_CH_1                      = 0x1A,
    RAZER_LED_ID_ARGB_CH_2                      = 0x1B,
    RAZER_LED_ID_ARGB_CH_3                      = 0x1C,
    RAZER_LED_ID_ARGB_CH_4                      = 0x1D,
    RAZER_LED_ID_ARGB_CH_5                      = 0x1E,
    RAZER_LED_ID_ARGB_CH_6                      = 0x1F,
};

/*---------------------------------------------------------*\
| Razer Matrix Type                                         |
\*---------------------------------------------------------*/
enum
{
    RAZER_MATRIX_TYPE_STANDARD                  = 0,
    RAZER_MATRIX_TYPE_EXTENDED                  = 1,
    RAZER_MATRIX_TYPE_LINEAR                    = 2,
    RAZER_MATRIX_TYPE_EXTENDED_ARGB             = 3,
    RAZER_MATRIX_TYPE_CUSTOM                    = 4,
};

/*---------------------------------------------------------*\
| Razer Keyboard Layout                                     |
\*---------------------------------------------------------*/
enum
{
    RAZER_KEYBOARD_LAYOUT_NONE                     = 0,
    RAZER_KEYBOARD_LAYOUT_US                       = 1,
    RAZER_KEYBOARD_LAYOUT_GREEK                    = 2,
    RAZER_KEYBOARD_LAYOUT_GERMAN                   = 3,
    RAZER_KEYBOARD_LAYOUT_FRENCH                   = 4,
    RAZER_KEYBOARD_LAYOUT_RUSSIAN                  = 5,
    RAZER_KEYBOARD_LAYOUT_UK                       = 6,
    RAZER_KEYBOARD_LAYOUT_NORDIC                   = 7,
    RAZER_KEYBOARD_LAYOUT_CHT                      = 8,
    RAZER_KEYBOARD_LAYOUT_KOREAN                   = 9,
    RAZER_KEYBOARD_LAYOUT_TURKISH                  = 10,
    RAZER_KEYBOARD_LAYOUT_THAILAND                 = 11,
    RAZER_KEYBOARD_LAYOUT_JAPAN                    = 12,
    RAZER_KEYBOARD_LAYOUT_PORTUGESE_BRAZIL         = 13,
    RAZER_KEYBOARD_LAYOUT_SPANISH_LATIN_AMERICAN   = 14,
    RAZER_KEYBOARD_LAYOUT_SWISS                    = 15,
    RAZER_KEYBOARD_LAYOUT_SPANISH_EUR              = 16,
    RAZER_KEYBOARD_LAYOUT_ITALIAN                  = 17,
    RAZER_KEYBOARD_LAYOUT_PORTUGESE_PORTUGA        = 18,
    RAZER_KEYBOARD_LAYOUT_HEBREW                   = 19,
    RAZER_KEYBOARD_LAYOUT_ARABIC                   = 20,
};

/*---------------------------------------------------------*\
| Razer Layout Type                                         |
\*---------------------------------------------------------*/
enum
{
    RAZER_LAYOUT_TYPE_NONE                      = 0x00,
    RAZER_LAYOUT_TYPE_ANSI                      = 0x01,
    RAZER_LAYOUT_TYPE_ISO                       = 0x02,
    RAZER_LAYOUT_TYPE_JIS                       = 0x04,

    RAZER_LAYOUT_TYPE_ALL                        = RAZER_LAYOUT_TYPE_ANSI | RAZER_LAYOUT_TYPE_ISO | RAZER_LAYOUT_TYPE_JIS
};

/*---------------------------------------------------------*\
| Razer Keyboard Variant                                    |
\*---------------------------------------------------------*/
enum
{
    RAZER_KEYBOARD_VARIANT_BLACK                = 0x00,
    RAZER_KEYBOARD_VARIANT_MERCURY              = 0x82,
};

/*---------------------------------------------------------*\
| Razer Keyboard Configuration                              |
\*---------------------------------------------------------*/
enum
{
    RAZER_SWITCH_OPTIMIZATION_GAMING = 0x00000000,
    RAZER_SWITCH_OPTIMIZATION_TYPING = 0x28001400,
};

enum
{
    RAZER_KEYBOARD_POLLING_RATE_125HZ  = 0x40,
    RAZER_KEYBOARD_POLLING_RATE_250HZ  = 0x20,
    RAZER_KEYBOARD_POLLING_RATE_500HZ  = 0x10,
    RAZER_KEYBOARD_POLLING_RATE_1000HZ = 0x08,
    RAZER_KEYBOARD_POLLING_RATE_2000HZ = 0x04,
    RAZER_KEYBOARD_POLLING_RATE_4000HZ = 0x02,
    RAZER_KEYBOARD_POLLING_RATE_8000HZ = 0x01,
};

/*---------------------------------------------------------*\
| Razer Mouse Configuration                                 |
\*---------------------------------------------------------*/

#define RAZER_MAX_SENSITIVITY_STAGES 5
enum
{
    RAZER_MOUSE_POLLING_RATE_125HZ     = 0x08,
    RAZER_MOUSE_POLLING_RATE_500HZ     = 0x02,
    RAZER_MOUSE_POLLING_RATE_1000HZ    = 0x01,
};

enum
{
    RAZER_SMART_TRACKING_MODE_REGULAR           = 0x01,
    RAZER_SMART_TRACKING_MODE_ASYMMETRIC        = 0x02,
    RAZER_SMART_TRACKING_MODE_MANUAL            = 0x03,
    RAZER_SMART_TRACKING_MODE_MANUAL_ASYMMETRIC = 0x04,
};

#define RAZER_MAX_SENSITIVITY_STAGES 5
struct razer_sensitivity_stage_data
{
    unsigned char active_stage;
    unsigned char number_of_stages;
    unsigned short x[RAZER_MAX_SENSITIVITY_STAGES];
    unsigned short y[RAZER_MAX_SENSITIVITY_STAGES];
};

/*---------------------------------------------------------*\
| Razer Report Type (taken from OpenRazer)                  |
\*---------------------------------------------------------*/
struct razer_rgb
{
    unsigned char r,g,b;
};

union transaction_id_union
{
    unsigned char id;
    struct transaction_parts
    {
        unsigned char device : 3;
        unsigned char id : 5;
    } parts;
};

union command_id_union
{
    unsigned char id;
    struct command_id_parts
    {
        unsigned char direction : 1;
        unsigned char id : 7;
    } parts;
};

PACK(struct razer_report
{
    unsigned char               report_id;
    unsigned char               status;
    union transaction_id_union  transaction_id;
    unsigned short              remaining_packets;
    unsigned char               protocol_type;
    unsigned char               data_size;
    unsigned char               command_class;
    union command_id_union      command_id;
    unsigned char               arguments[80];
    unsigned char               crc;
    unsigned char               reserved;
});

/*---------------------------------------------------------*\
| Razer ARGB Report Type (taken from OpenRazer)             |
\*---------------------------------------------------------*/
PACK(struct razer_argb_report
{
    unsigned char hid_id;
    unsigned char report_id;
    unsigned char channel_1;
    unsigned char channel_2;
    unsigned char pad;
    unsigned char last_idx;
    unsigned char color_data[315];
});

class RazerController
{
public:
    RazerController(hid_device* dev_handle, hid_device* dev_argb_handle, const char* path, unsigned short pid, std::string dev_name);
    ~RazerController();

    unsigned int            GetDeviceIndex();
    device_type             GetDeviceType();
    std::string             GetDeviceLocation();
    std::string             GetFirmwareString();
    std::string             GetName();
    std::string             GetSerialString();

    unsigned char           GetKeyboardLayoutType();
    std::string             GetKeyboardLayoutName();
    std::string             GetVariantName();

    void                    SetBrightness(unsigned char brightness);

    void                    SetLEDs(RGBColor* colors);
    void                    SetAddressableZoneSizes(unsigned char zone_1_size, unsigned char zone_2_size, unsigned char zone_3_size, unsigned char zone_4_size, unsigned char zone_5_size, unsigned char zone_6_size);

    void                    SetModeBreathingRandom();
    void                    SetModeBreathingOneColor(unsigned char red, unsigned char grn, unsigned char blu);
    void                    SetModeBreathingTwoColors(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    void                    SetModeOff();
    void                    SetModeSpectrumCycle();
    void                    SetModeStatic(unsigned char red, unsigned char grn, unsigned char blu);
    void                    SetModeWave(unsigned char direction);
    void                    SetModeRippleRandom();
    void                    SetModeRippleColor(unsigned char red, unsigned char grn, unsigned char blu);
    void                    SetModeStarlightRandom();
    void                    SetModeStarlightOneColor(unsigned char red, unsigned char grn, unsigned char blu);
    void                    SetModeStarlightTwoColors(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    void                    SetModeReactive(unsigned char red, unsigned char grn, unsigned char blu);

    bool                    SupportsReactive();
    bool                    SupportsWave();
    bool                    SupportsRipple();
    bool                    SupportsStarlight();

    // Functions for configuring keyboards
    bool                    GetKeyboardGamingMode();
    void                    SetKeyboardGamingMode(bool state);

    unsigned int            GetKeyboardSwitchOptimization();
    void                    SetKeyboardSwitchOptimization(unsigned int optimization);

    unsigned char           GetKeyboardPollingRate();
    void                    SetKeyboardPollingRate(unsigned char rate);

    // Functions for configuring mice
    unsigned char           GetMousePollingRate();
    void                    SetMousePollingRate(unsigned char rate);

    bool                    GetMouseLeftHandedMode();
    void                    SetMouseLeftHandedMode(bool left_handed);

    unsigned char           GetSmartTrackingSurface();
    void                    SetSmartTrackingSurface(unsigned char surface_index);

    unsigned char           GetSmartTrackingMode(unsigned char *offset);
    void                    SetSmartTrackingMode(unsigned char mode, unsigned char offset);

    void                    GetSensitivity(unsigned short* x, unsigned short* y);
    void                    SetSensitivity(unsigned short x, unsigned short y);

    void                    GetSensitivityStages(struct razer_sensitivity_stage_data *stages);
    void                    SetSensitivityStages(struct razer_sensitivity_stage_data *stages);

    // Functions for configuring wireless devices
    unsigned short          GetWirelessPowerSavingTime();
    void                    SetWirelessPowerSavingTime(unsigned short seconds);

    unsigned short          GetDimLightingConfiguration();
    void                    SetDimLightingConfiguration(unsigned short seconds);

    unsigned char           GetBatteryLevel();
    bool                    IsCharging();
    bool                    IsWirelessDevicePresent();

private:
    hid_device*             dev;
    hid_device*             dev_argb;
    unsigned short          dev_pid;

    /*---------------------------------------------------------*\
    | Device-specific protocol settings                         |
    \*---------------------------------------------------------*/
    unsigned char           dev_transaction_id;
    unsigned char           dev_led_id;

    /*---------------------------------------------------------*\
    | Device information strings                                |
    \*---------------------------------------------------------*/
    std::string             firmware_version;
    std::string             location;
    std::string             name;
    device_type             type;

    /*---------------------------------------------------------*\
    | Device Information                                        |
    \*---------------------------------------------------------*/
    bool                    is_wireless;

    /*---------------------------------------------------------*\
    | Index of device in Razer device list                      |
    \*---------------------------------------------------------*/
    unsigned int            device_index;

    /*---------------------------------------------------------*\
    | HID report index for request and response                 |
    \*---------------------------------------------------------*/
    unsigned char           report_index;
    unsigned char           response_index;

    /*---------------------------------------------------------*\
    | Matrix type                                               |
    \*---------------------------------------------------------*/
    unsigned char           matrix_type;

    /*---------------------------------------------------------*\
    | Functions for setting mode of extended matrix             |
    \*---------------------------------------------------------*/
    unsigned char get_extended_matrix_brightness(unsigned char led);
    void set_extended_matrix_brightness(unsigned char storage, unsigned char led, unsigned char brightness);

    void set_extended_matrix(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);

    unsigned char get_extended_matrix_mode(razer_report *report, unsigned char led);
    void set_extended_matrix_mode_off(unsigned char storage, unsigned char led);
    void set_extended_matrix_mode_static(unsigned char storage, unsigned char led, unsigned char r, unsigned char g, unsigned char b);
    void set_extended_matrix_mode_breathing_random(unsigned char storage, unsigned char led);
    void set_extended_matrix_mode_breathing_color(unsigned char storage, unsigned char led, unsigned char r, unsigned char g, unsigned char b);
    void set_extended_matrix_mode_breathing_dual_color(unsigned char storage, unsigned char led, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    void set_extended_matrix_mode_spectrum_cycling(unsigned char storage, unsigned char led);
    void set_extended_matrix_mode_wave(unsigned char storage, unsigned char led, unsigned char direction, unsigned short delay);
    void set_extended_matrix_mode_reactive(unsigned char storage, unsigned char led, unsigned char duration, unsigned char r, unsigned char g, unsigned char b);
    void set_extended_matrix_mode_ripple_random(unsigned char storage, unsigned char led, unsigned short delay);
    void set_extended_matrix_mode_ripple_color(unsigned char storage, unsigned char led, unsigned short delay, unsigned char r, unsigned char g, unsigned char b);
    void set_extended_matrix_mode_starlight_random(unsigned char storage, unsigned char led, unsigned char duration);
    void set_extended_matrix_mode_starlight_color(unsigned char storage, unsigned char led, unsigned char duration, unsigned char r, unsigned char g, unsigned char b);
    void set_extended_matrix_mode_starlight_dual_color(unsigned char storage, unsigned char led, unsigned char duration, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    void set_extended_matrix_mode_custom();
    void set_extended_matrix_mode_fire(unsigned char storage, unsigned char led);

    /*---------------------------------------------------------*\
    | Private functions based on OpenRazer                      |
    \*---------------------------------------------------------*/
    unsigned char           razer_calculate_crc(razer_report* report);
    razer_report            razer_create_report(unsigned char command_class, unsigned char command_id, unsigned char data_size);
    razer_report            razer_create_response();

    razer_report            razer_create_addressable_size_report(unsigned char zone_1_size, unsigned char zone_2_size, unsigned char zone_3_size, unsigned char zone_4_size, unsigned char zone_5_size, unsigned char zone_6_size);
    razer_report            razer_create_addressable_startup_detect_report(bool enable);
    razer_report            razer_create_brightness_standard_report(unsigned char variable_storage, unsigned char led_id, unsigned char brightness);
    razer_argb_report       razer_create_custom_frame_argb_report(unsigned char row_index, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_custom_frame_linear_report(unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_custom_frame_standard_matrix_report(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_device_mode_report(unsigned char mode, unsigned char param);
    razer_report            razer_create_mode_breathing_one_color_standard_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char red, unsigned char grn, unsigned char blu);
    razer_report            razer_create_mode_breathing_random_standard_matrix_report(unsigned char variable_storage, unsigned char led_id);
    razer_report            razer_create_mode_breathing_two_colors_standard_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    razer_report            razer_create_mode_custom_standard_matrix_report(unsigned char variable_storage);
    razer_report            razer_create_mode_none_standard_matrix_report(unsigned char variable_storage, unsigned char led_id);
    razer_report            razer_create_mode_spectrum_cycle_standard_matrix_report(unsigned char variable_storage, unsigned char led_id);
    razer_report            razer_create_mode_static_standard_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char red, unsigned char grn, unsigned char blu);
    razer_report            razer_create_mode_wave_standard_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char direction);
    razer_report            razer_create_set_led_effect_report(unsigned char variable_storage, unsigned char led_id, unsigned char effect);
    razer_report            razer_create_set_led_rgb_report(unsigned char variable_storage, unsigned char led_id, unsigned char* rgb_data);

    std::string             razer_get_firmware();
    std::string             razer_get_serial();
    void                    razer_get_keyboard_info(unsigned char* layout, unsigned char* variant);

    void                    razer_set_brightness(unsigned char brightness);
    void                    razer_set_custom_frame(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);

    void                    razer_set_device_mode(unsigned char device_mode);
    
    void                    razer_set_mode_breathing_random();
    void                    razer_set_mode_breathing_one_color(unsigned char red, unsigned char grn, unsigned char blu);
    void                    razer_set_mode_breathing_two_colors(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
    void                    razer_set_mode_custom();
    void                    razer_set_mode_none();
    void                    razer_set_mode_spectrum_cycle();
    void                    razer_set_mode_static(unsigned char red, unsigned char grn, unsigned char blu);
    void                    razer_set_mode_wave(unsigned char direction);

    int                     razer_usb_receive(razer_report* report);
    int                     razer_usb_send(razer_report* report);
    int                     razer_usb_send_argb(razer_argb_report* report);

};
