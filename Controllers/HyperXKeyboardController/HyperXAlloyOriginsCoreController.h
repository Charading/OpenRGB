/*------------------------------------------*\
|  HyperXAlloyOriginsCoreController.h        |
|                                            |
|  Definitions and types for HyperX Alloy    |
|  Origins Core RGB Keyboard lighting        |
|  controller                                |
|                                            |
|  Volodymyr Nazarchuk (Vavooon) 4/28/2021   |
\*------------------------------------------*/

#include "RGBController.h"
#include "hsv.h"

#include <string>
#include <hidapi/hidapi.h>

#pragma once

enum
{
   HYPERX_AOC_MODE_DIRECT         = 0x00,
   HYPERX_AOC_MODE_BREATHING      = 0x01,
   HYPERX_AOC_MODE_SWIPE          = 0x02,
};

enum
{
   HYPERX_AOC_SPEED_MIN           = 0x0,
   HYPERX_AOC_SPEED_MAX           = 0x2,
};

class HyperXAlloyOriginsCoreController
{
public:
    HyperXAlloyOriginsCoreController(hid_device* dev_handle, hid_device_info* dev_info);
    ~HyperXAlloyOriginsCoreController();

    std::string     GetDeviceLocation();
    std::string     GetSerialString();
    std::string     GetFirmwareVersion();

    void SetLEDs(std::vector<RGBColor> colors, unsigned char mode);
    void SetMode(int mode_value, unsigned int speed, std::vector<RGBColor> colors);

private:
    hid_device*     dev;
    std::string     location;
    std::string     firmware_version;
    RGBColor        color;
    hsv_t           hsv;
    unsigned int    hue;
    unsigned char   saturation;
    bool            isDimming;
    unsigned char   speed;
};
