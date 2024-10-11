/*---------------------------------------------------------*\
| SonyGamepadControllerDetect.cpp                           |
|                                                           |
|   Detector for Sony Gamepads                              |
|                                                           |
|   Pol Rius (alpemwarrior)                     24 Sep 2020 |
|   Flora Aubry                                 01 Jul 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "RGBController_SonyDS4.h"
#include "RGBController_SonyDualSense.h"

#define SONY_VID                        0x054C

#define SONY_DS4_V1_PID                 0x05C4
#define SONY_DS4_V2_PID                 0x09CC
#define SONY_DS4_RECEIVER_PID           0x0BA0
#define SONY_DUALSENSE_PID              0x0CE6

GENERIC_HOTPLUGGABLE_DETECTOR(DetectSonyDS4Controllers, SonyDS4Controller, RGBController_SonyDS4)

static Controllers DetectSonyDualSenseControllers(hid_device_info* info, const std::string&)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        bool is_bluetooth = info->interface_number    == -1;
        SonyDualSenseController*      controller      = new SonyDualSenseController(dev, info->path, is_bluetooth);
        RGBController_SonyDualSense*  rgb_controller  = new RGBController_SonyDualSense(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR("Sony DualShock 4", DetectSonyDS4Controllers,       SONY_VID, SONY_DS4_V1_PID);
REGISTER_HID_DETECTOR("Sony DualShock 4", DetectSonyDS4Controllers,       SONY_VID, SONY_DS4_V2_PID);
REGISTER_HID_DETECTOR("Sony DualShock 4", DetectSonyDS4Controllers,       SONY_VID, SONY_DS4_RECEIVER_PID);
REGISTER_HID_DETECTOR("Sony DualSense",   DetectSonyDualSenseControllers, SONY_VID, SONY_DUALSENSE_PID);
