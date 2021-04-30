/*-------------------------------------------------------------------*\
|  CMARGBController.h                                                 |
|                                                                     |
|  Driver for Coolermaster RGB USB Controller                         |
|                                                                     |
|  Nic W (midgetspy)        13th Apr 2021                             |
|                                                                     |
|  Simple RGB device with 5 modes                                     |
|                                                                     |
\*-------------------------------------------------------------------*/

#include <string>
#include <hidapi/hidapi.h>
#include "RGBController.h"  //Needed to set the direct mode

#pragma once

#define CM_RGBC_SPEED_NONE 0x05
#define CM_RGBC_BRIGHTNESS_OFF 0x03

enum
{
    CM_RGBC_MODE_STATIC          = 0x00,
    CM_RGBC_MODE_BREATHING       = 0x01,
    CM_RGBC_MODE_COLOR_CYCLE     = 0x02,
    CM_RGBC_MODE_STAR            = 0x03,
    CM_RGBC_MODE_MULTIPLE        = 0x04,
    CM_RGBC_MODE_OFF             = 0xFE,
};

enum
{
    CM_RGBC_BRIGHTNESS_1          = 0x3F,
    CM_RGBC_BRIGHTNESS_2          = 0x5A,
    CM_RGBC_BRIGHTNESS_3          = 0x7F,
    CM_RGBC_BRIGHTNESS_4          = 0x99,
    CM_RGBC_BRIGHTNESS_5          = 0xFF,
};

enum
{
    CM_RGBC_SPEED_SLOWEST         = 0x00,
    CM_RGBC_SPEED_SLOW            = 0x01,
    CM_RGBC_SPEED_NORMAL          = 0x02,
    CM_RGBC_SPEED_FAST            = 0x03,
    CM_RGBC_SPEED_FASTEST         = 0x04,
};

static unsigned char CM_RGBC_SPEED_BREATHING[5]   { 0x3C, 0x37, 0x31, 0x2C, 0x26 };
static unsigned char CM_RGBC_SPEED_STAR[5]        { 0x46, 0x41, 0x3C, 0x37, 0x32 };
static unsigned char CM_RGBC_SPEED_COLOR_CYCLE[5] { 0x96, 0x8C, 0x80, 0x6E, 0x68 };

class CMRGBController
{
public:
    CMRGBController(hid_device* dev_handle, char *_path);
    ~CMRGBController();

    std::string GetDeviceName();
    std::string GetSerial();
    std::string GetLocation();

    unsigned char       GetMode();
    void                SetMode(unsigned char mode, unsigned char speed, RGBColor colour, bool random_colours);
private:
    std::string         device_name;
    std::string         serial;
    std::string         location;
    hid_device*         dev;
};
