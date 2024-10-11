/*---------------------------------------------------------*\
| LenovoUSBControllerDetect.cpp                             |
|                                                           |
|   Detector for Lenovo USB devices                         |
|                                                           |
|   Cooper Hall (geobot19)                      17 Apr 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "LenovoDevices.h"
#include "RGBController_LenovoUSB.h"
#include "RGBController_Lenovo_Gen7_8.h"

/*-----------------------------------------------------*\
| vendor IDs                                            |
\*-----------------------------------------------------*/
#define ITE_VID                                 0x048D

/*-----------------------------------------------------*\
| Interface, Usage, and Usage Page                      |
\*-----------------------------------------------------*/
enum
{
    LENOVO_PAGE   = 0xFF89,
    LENOVO_USAGE = 0x07
};

static Controllers DetectLenovoLegionUSBControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        LenovoUSBController*     controller      = new LenovoUSBController(dev, info->path, info->product_id);
        RGBController_LenovoUSB* rgb_controller  = new RGBController_LenovoUSB(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectLenovoLegionUSBControllersGen7And8(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LenovoGen7And8USBController*    controller      = new LenovoGen7And8USBController(dev, info->path, info->product_id);
        LenovoRGBController_Gen7_8*     rgb_controller  = new LenovoRGBController_Gen7_8(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_PU("Lenovo Legion Y740",      DetectLenovoLegionUSBControllers,         ITE_VID, LEGION_Y740,   LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7 Gen 5",   DetectLenovoLegionUSBControllers,         ITE_VID, LEGION_Y750,   LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7S Gen 5",  DetectLenovoLegionUSBControllers,         ITE_VID, LEGION_Y750S,  LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7 Gen 6",   DetectLenovoLegionUSBControllers,         ITE_VID, LEGION_Y760,   LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7S Gen 6",  DetectLenovoLegionUSBControllers,         ITE_VID, LEGION_Y760S,  LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7 Gen 7",   DetectLenovoLegionUSBControllersGen7And8, ITE_VID, LEGION_7GEN7,  LENOVO_PAGE, LENOVO_USAGE);
REGISTER_HID_DETECTOR_PU("Lenovo Legion 7 Gen 8",   DetectLenovoLegionUSBControllersGen7And8, ITE_VID, LEGION_7GEN8,  LENOVO_PAGE, LENOVO_USAGE);
