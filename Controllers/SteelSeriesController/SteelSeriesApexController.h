/*-----------------------------------------*\
|  SteelSeriesApexController.h              |
|                                           |
|  Definitions and types for SteelSeries    |
|  Apex 7 Keyboard lighting controller      |
|                                           |
|  Eric Samuelson (edbgon) 7/5/2020         |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
    APEX7_PACKET_ID_DIRECT             = 0x3a,     /* Direct mode                */
};


enum
{
    APEX7_DIRECTION_RIGHT              = 0x00,
};

enum
{
    APEX7_MODE_WAVE                    = 0x00,
    APEX7_MODE_BREATHING               = 0x02,
};

enum
{
    APEX7_REACTIVE_MODE_TRIGGER        = 0x03,
};

enum
{
    APEX7_COLOR_MODE_SINGLE            = 0x00,
    APEX7_COLOR_MODE_DUAL              = 0x01,
    APEX7_COLOR_MODE_SPECTRUM          = 0x02
};

class SteelSeriesApexController
{
public:
    SteelSeriesApexController(hid_device* dev_handle);
    ~SteelSeriesApexController();

    void SetMode
        (
        unsigned char mode,
        unsigned char direction,
        unsigned char speed,
        std::vector<RGBColor> colors
        );

    void SetLEDsDirect(std::vector<RGBColor> colors);

private:
    hid_device*             dev;
    unsigned char           active_mode;
    unsigned char           active_direction;
    unsigned char           active_speed;

    void    SelectProfile
                (
                unsigned char   profile
                );
};