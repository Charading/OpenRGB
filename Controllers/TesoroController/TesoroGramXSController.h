/*-----------------------------------------*\
|  TesoroGramXSController.h                 |
|                                           |
|  RGB Interface for Tesoro Gram XS         |
|  Keyboard                                 |
|                                           |
|  Ondřej Niesner 8/31/2023                 |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    GRAM_XS_KEY_COUNT           = 104,
    GRAM_XS_RANDOM_COLOR        = 0x02,
    GRAM_XS_DEFAULT_PROFILE     = 0x04
};

enum
{
    GRAM_XS_PACKET_SIZE         = 264,
    GRAM_XS_PACKET_R            = 8,
    GRAM_XS_PACKET_G_1          = 152,
    GRAM_XS_PACKET_G_2          = 8,
    GRAM_XS_PACKET_B            = 40,
    GRAM_XS_PACKET_COLOR_SIZE   = 144,
    GRAM_XS_PACKET_HEADER_SIZE  = 8
};

enum
{
    GRAM_XS_MODE_DIRECT         = 0xFF, /* Dummy value, this mode does not exist */
    GRAM_XS_MODE_STATIC         = 0x00,
    GRAM_XS_MODE_TRIGGER        = 0x01,
    GRAM_XS_MODE_FIREWORK       = 0x02,
    GRAM_XS_MODE_BREATHING      = 0x03,
    GRAM_XS_MODE_STAR_SHINING   = 0x04,
    GRAM_XS_MODE_CYCLING        = 0x05,
    GRAM_XS_MODE_RIPPLE         = 0x06,
    GRAM_XS_MODE_WAVE           = 0x07,
    GRAM_XS_MODE_SPIRAL         = 0x08
};

enum
{
    GRAM_XS_BRIGHTNESS          = 0x04,
    GRAM_XS_BRIGHTNESS_MIN      = 0x00,
    GRAM_XS_BRIGHTNESS_MAX      = 0x04
};

class TesoroGramXSController
{
public:
    TesoroGramXSController(hid_device *dev_handle, const char *path);
    ~TesoroGramXSController();

    std::string     GetLocation();
    std::string     GetSerialString();

    void    SetMode
            (
            unsigned char mode,
            unsigned char brightness,
            unsigned char color_mode,
            std::vector<RGBColor> colors
            );
    
    void    UpdateLeds
            (
            std::vector<RGBColor> colors,
            bool save_to_flash = false
            );

private:
    std::string location;
    hid_device  *dev;
};
