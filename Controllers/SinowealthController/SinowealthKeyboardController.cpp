/*-----------------------------------------*\
|  SinowealthController.cpp                 |
|                                           |
|  Definitions and types for Sinowealth     |
|  mice, including Glorious                 |
|                                           |
|  Niels Westphal (crashniels) 20/5/2020    |
\*-----------------------------------------*/

#include "SinowealthKeyboardController.h"
#include <cstring>

static unsigned char handshake_packet[] = {0x06, 0x03, 0x01, 0x00, 0x08, 0x04};

static unsigned char asdasdasdasdasdasdasdasdasda[] = { 0x06, 0x03, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x5a, 0xa5, 0x03, 0x03, 0x00, 0x00, 0x00, 0x01, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
                                                        0x55, 0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x24, 0x07, 0x23, 0x07, 0x23,
                                                        0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23,
                                                        0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23,
                                                        0x07, 0x23, 0x00, 0x10, 0x00, 0x10, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44,
                                                        0x07, 0x44, 0x07, 0x44, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5, 0x03, 0x03};

static unsigned char mode_brightness_speed_packet[] = { 0x06, 0x03, 0xb6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x5a, 0xa5, 0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00,
                                                        0x55, 0x55, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x20, 0x00, 0x44, 0x07, 0x30,
                                                        0x07, 0x23, 0x00, 0x23, 0x00, 0x23, 0x07, 0x33, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23,
                                                        0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23, 0x07, 0x23,
                                                        0x07, 0x23, 0x00, 0x10, 0x00, 0x10, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44, 0x07, 0x44,
                                                        0x07, 0x44, 0x07, 0x44, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5a, 0xa5, 0x03, 0x03};

static unsigned int keys_tkl[] = { 0x22, 0x24, 0x26, 0x27, 0x29, 0x2b, 0x2d, 0x2e, 0x2f,
                                   0x30, 0x31, 0x32, 0x37, 0x39, 0x3b, 0x3c, 0x3e,
                                   0x40, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x4c, 0x4e,
                                   0x50, 0x51, 0x53, 0x55, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c,
                                   0x61, 0x63, 0x65, 0x66, 0x68, 0x6a, 0x6c, 0x6d, 0x6e, 0x6f,
                                   0x70, 0x71, 0x76, 0x78, 0x7a, 0x7b, 0x7d, 0x7f,
                                   0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x8b, 0x8d, 0x8f,
                                   0x90, 0x92, 0x94, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b,
                                   0xa0, 0xa2, 0xa4, 0xa5, 0xa7, 0xa9, 0xab, 0xac, 0xad, 0xae, 0xaf,
                                   0xb0, 0xb5, 0xb7, 0xb9, 0xba, 0xbc, 0xbe,
                                   0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xca, 0xcc, 0xce, 0xcf,
                                   0xd1, 0xd3, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdf,
                                   0xe1, 0xe3, 0xe4, 0xe6, 0xe8, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
                                   0xf4, 0xf6, 0xf8, 0xf9, 0xfb, 0xfd, 0xff,
                                   0x100, 0x101, 0x102, 0x103, 0x104, 0x109, 0x10b, 0x10d, 0x10e,
                                   0x110, 0x112, 0x114, 0x115, 0x116, 0x117, 0x118, 0x119, 0x11e,
                                   0x120, 0x122, 0x123, 0x125, 0x127, 0x129, 0x12a, 0x12b, 0x12c, 0x12d, 0x12e,
                                   0x133, 0x135, 0x137, 0x138, 0x13a, 0x13c, 0x13e, 0x13f,
                                   0x140, 0x141, 0x142, 0x143, 0x148, 0x14a, 0x14c, 0x14d, 0x14f,
                                   0x151, 0x153, 0x154, 0x155, 0x156, 0x157, 0x158, 0x15d, 0x15f,
                                   0x161, 0x162, 0x164, 0x166, 0x168, 0x169, 0x16a, 0x16b, 0x16c, 0x16d,
                                   0x172, 0x174, 0x176, 0x177, 0x179, 0x17b, 0x17d, 0x17e, 0x17f,
                                   0x180, 0x181, 0x182, 0x187, 0x189, 0x18b, 0x18c, 0x18e,
                                   0x190, 0x192, 0x193, 0x194, 0x195, 0x196, 0x197, 0x19c, 0x19e,
                                   0x1a0, 0x1a1, 0x1a3, 0x1a5, 0x1a7, 0x1a8, 0x1a9, 0x1aa, 0x1ab, 0x1ac,
                                   0x1b1, 0x1b3, 0x1b5, 0x1b6, 0x1b8, 0x1ba, 0x1bc, 0x1bd, 0x1be, 0x1bf,
                                   0x1c0, 0x1c1, 0x1c6, 0x1c8, 0x1ca, 0x1cb, 0x1cd, 0x1cf,
                                   0x1d1, 0x1d2, 0x1d3, 0x1d4, 0x1d5, 0x1d6, 0x1db, 0x1dd, 0x1df,
                                   0x1e0, 0x1e2, 0x1e4, 0x1e6, 0x1e7, 0x1e8, 0x1e9, 0x1ea
                                 };

SinowealthKeyboardController::SinowealthKeyboardController(hid_device* dev_handle_id_4, hid_device* dev_handle_id_5, char *_path)
{
    dev_report_id_4 = dev_handle_id_4;
    dev_report_id_5 = dev_handle_id_5;

    led_count = 1;

    current_mode      = MODE_STATIC;
    current_speed     = SPEED_NORMAL;

    location = _path;
}

SinowealthKeyboardController::~SinowealthKeyboardController()
{
    hid_close(dev_report_id_4);
    hid_close(dev_report_id_5);
}

std::string SinowealthKeyboardController::GetLocation()
{
    return("HID: " + location);
}

unsigned int SinowealthKeyboardController::GetLEDCount()
{
    return 87;
}

std::string SinowealthKeyboardController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev_report_id_4, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}



void SinowealthKeyboardController::SetLEDColor(RGBColor* color_buf)
{
    //GetProfile();

    const int buffer_size = 1032;

    unsigned char usb_buf[buffer_size];
    memset(usb_buf, 0x00, sizeof(usb_buf));
    int offset = 0;

    usb_buf[offset] = 0x06;
    offset += 1;

    usb_buf[offset] = 0x08;
    offset += 1;

    usb_buf[offset] = 0xB8;
    offset += 2;

    usb_buf[offset] = 0x40;

    usb_buf[0x1D] = RGBGetRValue(color_buf[0]);
    usb_buf[0x1E] = RGBGetGValue(color_buf[0]);
    usb_buf[0x1F] = RGBGetBValue(color_buf[0]);

    auto size_of_keys_array = sizeof (keys_tkl)/ sizeof(int);

    for (int i = 0x00; i < size_of_keys_array; i++)
    {
        auto key_code = keys_tkl[i];
        usb_buf[key_code] = 0xff;
    }

    auto result = hid_send_feature_report(dev_report_id_5, usb_buf, sizeof(usb_buf));

    if (result == -1)
    {
        printf("Error sending read request!");
    }

    //SetBrightness(0);
}

void SinowealthKeyboardController::SetMode(unsigned char mode, unsigned char brightness, unsigned char speed, unsigned char color_mode, RGBColor* color_buf)
{
    const int buffer_size = 1032;

    int mode_byte_index = 0x15;
    const int speed_and_brightness_byte_index_start = 0x29; // Speed + brightnes level value, Seriously?

    auto color_mode_value = color_mode == MODE_COLORS_RANDOM ? 0x07 : 0x00; // 0x07 - Value to set random color mode

    unsigned char usb_buf[buffer_size];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    auto mode_brightness_speed_packet_length = sizeof(mode_brightness_speed_packet)/sizeof(char);

    for (int i = 0x00; i < mode_brightness_speed_packet_length; i++)
    {
        usb_buf[i] = mode_brightness_speed_packet[i];
    }

    usb_buf[mode_byte_index] = mode;

    int speed_and_brightness_byte_index = speed_and_brightness_byte_index_start + ((mode - 2) * 2);

    switch (mode)
    {
    case MODE_OFF:
        break;
    case MODE_RAINBOW:
        break;
    case MODE_FLASH_AWAY:
        break;
    case MODE_RAINDROPS:
        break;
    case MODE_RAINBOW_WHEEL:
        break;
    case MODE_RIPPLES_SHINING:
        break;
    case MODE_STARS_TWINKLE:
        break;
    case MODE_SHADOW_DISAPPEAR:
        break;
    case MODE_RETRO_SNAKE:
        break;
    case MODE_NEON_STREAM:
        break;
    case MODE_REACTION:
        break;
    case MODE_SINE_WAVE:
        break;
    case MODE_RETINUE_SCANNING:
        break;
    case MODE_ROTATING_WINDMILL:
        break;
    case MODE_COLORFUL_WATERFALL:
        break;
    case MODE_BLOSSOMING:
        break;
    case MODE_ROTATING_STORM:
        break;
    case MODE_COLLISION:
        break;
    case MODE_PERFECT:
        break;
    }

    int color_mode_byte_index = speed_and_brightness_byte_index - 1;

    usb_buf[speed_and_brightness_byte_index] = speed + brightness;
    usb_buf[color_mode_byte_index] = color_mode_value;

    hid_send_feature_report(dev_report_id_5, usb_buf, sizeof(usb_buf));
}



void SinowealthKeyboardController::GetProfile()
{
    static unsigned char starting_config[] = { 0x05, 0x83, 0xB6, 0x00, 0x00, 0x00};

    int actual;

    actual = hid_send_feature_report(dev_report_id_4, starting_config, 6);

    if (actual != 6)
    {
        printf("Error sending read request!");
    }
}
