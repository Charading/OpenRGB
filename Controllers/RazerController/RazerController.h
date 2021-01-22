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

/*-----------------------------------------------------*\
| Razer vendor ID                                       |
\*-----------------------------------------------------*/
#define RAZER_VID                               0x1532

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
|   List taken from OpenRazer                           |
|   Non-RGB keyboards were omitted from this list       |
\*-----------------------------------------------------*/
#define RAZER_BLACKWIDOW_CHROMA_PID             0x0203
#define RAZER_DEATHSTALKER_CHROMA_PID           0x0204
#define RAZER_BLADE_STEALTH_PID                 0x0205
#define RAZER_ORBWEAVER_CHROMA_PID              0x0207
#define RAZER_TARTARUS_CHROMA_PID               0x0208
#define RAZER_BLACKWIDOW_CHROMA_TE_PID          0x0209
#define RAZER_BLADE_QHD_PID                     0x020F
#define RAZER_BLADE_PRO_LATE_2016_PID           0x0210
#define RAZER_BLACKWIDOW_OVERWATCH_PID          0x0211
#define RAZER_BLACKWIDOW_X_CHROMA_PID           0x0216
#define RAZER_BLACKWIDOW_X_CHROMA_TE_PID        0x021A
#define RAZER_ORNATA_CHROMA_PID                 0x021E
#define RAZER_BLADE_STEALTH_LATE_2016_PID       0x0220
#define RAZER_BLACKWIDOW_CHROMA_V2_PID          0x0221
#define RAZER_BLADE_LATE_2016_PID               0x0224
#define RAZER_BLADE_PRO_2017_PID                0x0225
#define RAZER_HUNTSMAN_ELITE_PID                0x0226
#define RAZER_HUNTSMAN_PID                      0x0227
#define RAZER_BLACKWIDOW_ELITE_PID              0x0228
#define RAZER_CYNOSA_CHROMA_PID                 0x022A
#define RAZER_TARTARUS_V2_PID                   0x022B
#define RAZER_BLADE_STEALTH_MID_2017_PID        0x022D
#define RAZER_BLADE_PRO_2017_FULLHD_PID         0x022F
#define RAZER_BLADE_STEALTH_LATE_2017_PID       0x0232
#define RAZER_BLADE_2018_PID                    0x0233
#define RAZER_BLADE_PRO_2019_PID                0x0234
#define RAZER_BLACKWIDOW_LITE_PID               0x0235
#define RAZER_BLACKWIDOW_ESSENTIAL_PID          0x0237
#define RAZER_BLADE_STEALTH_2019_PID            0x0239
#define RAZER_BLADE_2019_ADV_PID                0x023A
#define RAZER_BLADE_2018_BASE_PID               0x023B
#define RAZER_CYNOSA_LITE_PID                   0x023F
#define RAZER_BLADE_2018_MERCURY_PID            0x0240
#define RAZER_BLACKWIDOW_2019_PID               0x0241
#define RAZER_HUNTSMAN_TE_PID                   0x0243
#define RAZER_BLADE_MID_2019_MERCURY_PID        0x0245
#define RAZER_BLADE_2019_BASE_PID               0x0246
#define RAZER_BLADE_STEALTH_LATE_2019_PID       0x024A
#define RAZER_BLADE_PRO_LATE_2019_PID           0x024C
#define RAZER_BLADE_STUDIO_EDITION_2019_PID     0x024D
#define RAZER_BLADE_STEALTH_EARLY_2020_PID      0x0252
#define RAZER_BLADE_15_ADV_2020_PID             0x0253
#define RAZER_BLADE_EARLY_2020_BASE_PID         0x0255
#define RAZER_BLADE_STEALTH_LATE_2020_PID       0x0259
#define RAZER_ORNATA_CHROMA_V2_PID              0x025D
#define RAZER_CYNOSA_V2_PID                     0x025E

/*-----------------------------------------------------*\
| Mouse product IDs                                     |
|   List taken from OpenRazer                           |
\*-----------------------------------------------------*/
#define RAZER_OROCHI_2011_PID                   0x0013
#define RAZER_DEATHADDER_3_5G_PID               0x0016
#define RAZER_ABYSSUS_1800_PID                  0x0020
#define RAZER_MAMBA_2012_WIRED_PID              0x0024
#define RAZER_MAMBA_2012_WIRELESS_PID           0x0025
#define RAZER_NAGA_2012_PID                     0x002E
#define RAZER_IMPERATOR_PID                     0x002F
#define RAZER_OUROBOROS_PID                     0x0032
#define RAZER_TAIPAN_PID                        0x0034
#define RAZER_NAGA_HEX_RED_PID                  0x0036
#define RAZER_DEATHADDER_2013_PID               0x0037
#define RAZER_DEATHADDER_1800_PID               0x0038
#define RAZER_OROCHI_2013_PID                   0x0039
#define RAZER_NAGA_2014_PID                     0x0040
#define RAZER_NAGA_HEX_PID                      0x0041
#define RAZER_ABYSSUS_PID                       0x0042
#define RAZER_DEATHADDER_CHROMA_PID             0x0043
#define RAZER_MAMBA_WIRED_PID                   0x0044
#define RAZER_MAMBA_WIRELESS_PID                0x0045
#define RAZER_MAMBA_TE_WIRED_PID                0x0046
#define RAZER_OROCHI_CHROMA_PID                 0x0048
#define RAZER_DIAMONDBACK_CHROMA_PID            0x004C
#define RAZER_DEATHADDER_2000_PID               0x004F
#define RAZER_NAGA_HEX_V2_PID                   0x0050
#define RAZER_NAGA_CHROMA_PID                   0x0053
#define RAZER_DEATHADDER_3500_PID               0x0054
#define RAZER_LANCEHEAD_WIRED_PID               0x0059
#define RAZER_LANCEHEAD_WIRELESS_PID            0x005A
#define RAZER_ABYSSUS_V2_PID                    0x005B
#define RAZER_DEATHADDER_ELITE_PID              0x005C
#define RAZER_ABYSSUS_2000_PID                  0x005E
#define RAZER_LANCEHEAD_TE_WIRED_PID            0x0060
#define RAZER_ATHERIS_RECEIVER_PID              0x0062
#define RAZER_BASILISK_PID                      0x0064
#define RAZER_NAGA_TRINITY_PID                  0x0067
#define RAZER_ABYSSUS_ELITE_DVA_EDITION_PID     0x006A
#define RAZER_ABYSSUS_ESSENTIAL_PID             0x006B
#define RAZER_MAMBA_ELITE_PID                   0x006C
#define RAZER_DEATHADDER_ESSENTIAL_PID          0x006E
#define RAZER_LANCEHEAD_WIRELESS_RECEIVER_PID   0x006F
#define RAZER_LANCEHEAD_WIRELESS_WIRED_PID      0x0070
#define RAZER_DEATHADDER_ESSENTIAL_WHITE_EDITION_PID    0x0071
#define RAZER_MAMBA_WIRELESS_RECEIVER_PID       0x0072
#define RAZER_MAMBA_WIRELESS_WIRED_PID          0x0073
#define RAZER_VIPER_PID                         0x0078
#define RAZER_VIPER_ULTIMATE_WIRED_PID          0x007A
#define RAZER_VIPER_ULTIMATE_WIRELESS_PID       0x007B
#define RAZER_DEATHADDER_V2_PRO_WIRED_PID       0x007C
#define RAZER_DEATHADDER_V2_PRO_WIRELESS_PID    0x007D
#define RAZER_BASILISK_X_HYPERSPEED_PID         0x0083
#define RAZER_DEATHADDER_V2_PID                 0x0084
#define RAZER_VIPER_MINI_PID                    0x008A
#define RAZER_DEATHADDER_V2_MINI_PID            0x008C

/*-----------------------------------------------------*\
| Accessory product IDs                                 |
|   List taken from OpenRazer                           |
\*-----------------------------------------------------*/
#define RAZER_FIREFLY_HYPERFLUX_PID             0x0068
#define RAZER_CORE_PID                          0x0215
#define RAZER_NOMMO_CHROMA_PID                  0x0517
#define RAZER_NOMMO_PRO_PID                     0x0518
#define RAZER_FIREFLY_PID                       0x0C00
#define RAZER_GOLIATHUS_CHROMA_PID              0x0C01
#define RAZER_GOLIATHUS_CHROMA_EXTENDED_PID     0x0C02
#define RAZER_CHROMA_MUG_PID                    0x0F07
#define RAZER_CHROMA_BASE_PID                   0x0F08
#define RAZER_CHROMA_HDK_PID                    0x0F09
#define RAZER_KRAKEN_KITTY_EDITION_PID          0x0F19
#define RAZER_MOUSE_BUNGEE_V3_CHROMA_PID        0x0F1D
#define RAZER_BASE_STATION_V2_CHROMA_PID        0x0F20

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

PACK(typedef struct razer_report
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
};)

class RazerController
{
public:
    RazerController(hid_device* dev_handle, const char* path, unsigned short pid, std::string dev_name);
    ~RazerController();

    unsigned int            GetDeviceIndex();
    device_type             GetDeviceType();
    std::string             GetDeviceLocation();
    std::string             GetFirmwareString();
    std::string             GetName();
    std::string             GetSerialString();

    void                    SetLEDs(RGBColor* colors);

    void                    SetModeSpectrumCycle();

private:
    hid_device*             dev;
    unsigned short          dev_pid;

    /*---------------------------------------------------------*\
    | Device information strings                                |
    \*---------------------------------------------------------*/
    std::string             firmware_version;
    std::string             location;
    std::string             name;
    device_type             type;

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
    | Private functions based on OpenRazer                      |
    \*---------------------------------------------------------*/
    unsigned char           razer_calculate_crc(razer_report* report);
    razer_report            razer_create_report(unsigned char command_class, unsigned char command_id, unsigned char data_size);
    razer_report            razer_create_response();

    razer_report            razer_create_brightness_extended_matrix_report(unsigned char variable_storage, unsigned char led_id, unsigned char brightness);
    razer_report            razer_create_brightness_standard_report(unsigned char variable_storage, unsigned char led_id, unsigned char brightness);
    razer_report            razer_create_custom_frame_linear_report(unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_custom_frame_extended_matrix_report(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_custom_frame_standard_matrix_report(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);
    razer_report            razer_create_mode_custom_extended_matrix_report();
    razer_report            razer_create_mode_custom_standard_matrix_report(unsigned char variable_storage);
    razer_report            razer_create_mode_spectrum_cycle_extended_matrix_report(unsigned char variable_storage, unsigned char led_id);
    razer_report            razer_create_mode_spectrum_cycle_standard_matrix_report(unsigned char variable_storage, unsigned char led_id);
    razer_report            razer_create_set_led_effect_report(unsigned char variable_storage, unsigned char led_id, unsigned char effect);
    razer_report            razer_create_set_led_rgb_report(unsigned char variable_storage, unsigned char led_id, unsigned char* rgb_data);

    std::string             razer_get_firmware();
    std::string             razer_get_serial();

    void                    razer_set_brightness(unsigned char brightness);
    void                    razer_set_custom_frame(unsigned char row_index, unsigned char start_col, unsigned char stop_col, unsigned char* rgb_data);

    void                    razer_set_mode_breathing();
    void                    razer_set_mode_custom();
    void                    razer_set_mode_none();
    void                    razer_set_mode_spectrum_cycle();
    void                    razer_set_mode_wave();

    int                     razer_usb_receive(razer_report* report);
    int                     razer_usb_send(razer_report* report);


};
