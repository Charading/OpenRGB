/*-------------------------------------------------------------------*\
|  ThermaltakeRiingQuadController.h                                   |
|                                                                     |
|  Driver for Thermaltake Riing Quad Controller                       |
|                                                                     |
|  Chris M (Dr_No)          15th Feb 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController.h"
#include <vector>
#include <hidapi/hidapi.h>

#pragma once

#define TT_QUAD_ZONES 5
#define TT_QUAD_PACKET_SIZE 193
#define TT_QUAD_INTERRUPT_TIMEOUT 250 //Time in Milliseconds
#define HID_MAX_STR 255

enum
{
    TT_QUAD_COMMAND_BYTE        = 1,
    TT_QUAD_FUNCTION_BYTE       = 2,
    TT_QUAD_ZONE_BYTE           = 3,
    TT_QUAD_MODE_BYTE           = 4,
    TT_QUAD_DATA_BYTE           = 5,
};

enum
{
    TT_QUAD_MODE_DIRECT         = 0x24
};

enum
{
    TT_QUAD_SPEED_EXTREME       = 0x00,
    TT_QUAD_SPEED_FAST          = 0x01,
    TT_QUAD_SPEED_NORMAL        = 0x02,
    TT_QUAD_SPEED_SLOW          = 0x03,
};


class ThermaltakeRiingQuadController
{
public:
    ThermaltakeRiingQuadController(hid_device* dev_handle, const char* path);
    ~ThermaltakeRiingQuadController();

    std::string     GetDeviceName();
    std::string     GetDeviceLocation();
    std::string     GetSerial();

    void            SetChannelLEDs(unsigned char channel, RGBColor * colors, unsigned int num_colors);
    void            SetMode(unsigned char mode, unsigned char speed);

private:
    hid_device*     dev;

    std::string     device_name;
    std::string     serial;
    std::string     location;
    unsigned char   current_mode;
    unsigned char   current_speed;

    void    SendInit();
    void    SendRGB(unsigned char port, unsigned char num_colors, unsigned char* color_data);

    void    SendFan();
    void    SendSave();
};
