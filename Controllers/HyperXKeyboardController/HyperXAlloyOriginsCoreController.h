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

class HyperXAlloyOriginsCoreController
{
public:
    HyperXAlloyOriginsCoreController(hid_device* dev_handle, hid_device_info* dev_info);
    ~HyperXAlloyOriginsCoreController();

    std::string     GetDeviceLocation();
    std::string     GetSerialString();
    std::string     GetFirmwareVersion();

    void SetLEDsDirect(std::vector<RGBColor> colors);
    void SetBreatheColor(RGBColor color);
    void Breathe(std::vector<RGBColor> colors);

private:
    hid_device*     dev;
    std::string     location;
    std::string     firmware_version;
    RGBColor        color;
    hsv_t           hsv;
    unsigned int    hue;
    unsigned char   saturation;
    bool            isDimming;
};
