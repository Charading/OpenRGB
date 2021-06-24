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
   HYPERX_AOC_SPEED_MIN           = 0x01,
   HYPERX_AOC_SPEED_MAX           = 0x07,
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
    void SetMode(int mode_value, unsigned int speed, std::vector<RGBColor> colors, matrix_map_type* matrix_map);

private:
    hid_device*     dev;
    std::string     location;
    std::string     firmware_version;
    RGBColor        color0;
    RGBColor        color1;
    RGBColor        cur_color;
    hsv_t           hsv;
    bool            isDimming;
    unsigned char   column;
    unsigned char   speed;
    unsigned char   brightness_upper_bound;
    unsigned char   brightness_lower_bound;
    matrix_map_type* matrix_map;
    unsigned int(*data)[19];
    std::vector<RGBColor> colors_;
    unsigned char buf[380];
    bool             color_end;
    int              iteration;
    bool             send;
};
