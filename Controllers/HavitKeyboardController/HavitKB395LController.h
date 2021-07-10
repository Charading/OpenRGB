/*-----------------------------------------*\
|  HavitKB395LController.h                  |
|                                           |
|  Definitions and types for Havit KB395L   |
|  keyboard lighting controller             |
|                                           |
|  Lisander Lopez 7/7/2021                  |
\*-----------------------------------------*/

#include "RGBController.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

/*-----------------------------------------*\
|   All Modes implemented for this keyboard |
\*-----------------------------------------*/
enum
{
    HAVIT_KB395L_MODE_STATIC             = 0x00,
};

/*-----------------------------------------*\
|   This variable maps the color map from   |
|   a binary RGB value to, the base color   |
|   understood by the protocol              |
\*-----------------------------------------*/
static unsigned char base_color_map[2][2][2] = 
{
    {
        {0x00, 0xE4},
        {0xE6, 0xE3}
    },
    {
        {0xE7, 0xE2},
        {0xE5, 0xE1}
    },
};

/*-----------------------------------------*\
|   This variable represents the color      |
|   indexes that are mapped from binary     |
|   RGB values understood by the protocol   |
\*-----------------------------------------*/
static unsigned char color_index_map[2][2][2] = 
{
    {
        {0x00, 0x04},
        {0x02, 0x05}
    },
    {
        {0x01, 0x06},
        {0x03, 0x07}
    },
};

class HavitKB395LController
{
public:
    HavitKB395LController(hid_device* dev_handle, const char* path);
    ~HavitKB395LController();

    std::string GetDeviceLocation();
    std::string GetSerialString();

    void SendStaticColor(unsigned char red, unsigned char blue, unsigned char green, unsigned char brightness);

private:
    hid_device*             dev;
    std::string             location;

};
