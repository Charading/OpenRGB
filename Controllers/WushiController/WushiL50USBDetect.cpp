/*---------------------------------------------------------*\
| WushiL50USBControllerDetect.cpp                           |
|                                                           |
|   Detector for Wushi L50                                  |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "WushiL50USBController.h"
#include "RGBController_WushiL50USB.h"
#include <hidapi.h>

/*-----------------------------------------------------*\
| Wushi vendor ID                                       |
\*-----------------------------------------------------*/
#define WUSHI_VID                               0x306F

/*-----------------------------------------------------*\
| Wushi device ID                                       |
\*-----------------------------------------------------*/
#define WUSHI_PID                               0x1234

static Controllers DetectWushiL50USBControllers(const hidapi_wrapper* wrapper, hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = wrapper->hid_open_path(info->path);
    if(dev)
    {
        WushiL50USBController*     controller      = new WushiL50USBController(wrapper, dev, info->path);
        RGBController_WushiL50USB* rgb_controller  = new RGBController_WushiL50USB(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_WRAPPED_DETECTOR("JSAUX RGB Docking Station", DetectWushiL50USBControllers, WUSHI_VID, WUSHI_PID);
