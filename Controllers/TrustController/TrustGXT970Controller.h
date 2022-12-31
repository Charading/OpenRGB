/*-----------------------------------------*\
|  TrustGXT970Controller.h                  |
|                                           |
|  Driver for Trust GXT 970                 |
|  controller - header file                 |
|                                           |
|  David Heidelberg 31/12/2022              |
\*-----------------------------------------*/

#include "RGBController.h"
#include <string>
#include <hidapi/hidapi.h>

#pragma once

#define TRUST_GXT_970_REPORT_SIZE       512 // original 64 CHECK! TODO
#define TRUST_GXT_970_NUMBER_OF_LEDS    1
#define TRUST_GXT_970_REPORT_ID         0x04

enum
{
    TRUST_GXT_970_OFF_MODE_VALUE            = 0x00,
    TRUST_GXT_970_RAINBOW_WAVE_MODE_VALUE   = 0x01,
    TRUST_GXT_970_STATIC_MODE_VALUE         = 0x02,
    TRUST_GXT_970_BREATHING_MODE_VALUE      = 0x02 + 0x01,
    TRUST_GXT_970_TAIL_MODE_VALUE           = 0x04,
    TRUST_GXT_970_NEON_MODE_VALUE           = 0x04 + 0x01,
    TRUST_GXT_970_RAINBOW_MODE_VALUE        = 0x04 + 0x02,
    TRUST_GXT_970_FLICKER_VALUE             = 0x04 + 0x02 + 0x01,
    TRUST_GXT_970_RANDOM_VALUE              = 0x08,
    TRUST_GXT_970_WAVE_VALUE                = 0x08 + 0x01,
    TRUST_GXT_970_RESPONSE_VALUE            = 0x08 + 0x02,
};

enum
{
    TRUST_GXT_970_BRIGHTNESS_MIN            = 0x10, // 25%
    TRUST_GXT_970_BRIGHTNESS_MAX            = 0x40 // 100%
};

enum
{
    TRUST_GXT_970_SPEED_MIN                 = 0x41,
    TRUST_GXT_970_SPEED_MAX                 = 0x43
};

enum
{
    TRUST_GXT_970_DPI_MIN                   = 0x01,
    TRUST_GXT_970_DPI_MAX                   = 0x3B
};

class TrustGXT970Controller
{
public:
    TrustGXT970Controller(hid_device* dev_handle, const hid_device_info& info);
    ~TrustGXT970Controller();

    std::string                 GetSerialString();
    std::string                 GetDeviceLocation();
    std::string                 GetFirmwareVersion();
    void                        SetMode(RGBColor color, unsigned char brightness, unsigned char speed, unsigned char mode_value);

protected:
    hid_device* dev;

private:
    std::string                 location;
    std::string                 serial_number;
    std::string                 version;
};
