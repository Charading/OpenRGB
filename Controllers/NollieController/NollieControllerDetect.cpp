/*---------------------------------------------------------*\
| NollieControllerDetect.cpp                                |
|                                                           |
|   Detector for Nollie                                     |
|                                                           |
|   Name (cnn1236661)                           25 Jun 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "NollieController.h"
#include "RGBController_Nollie.h"

static Controllers DetectNollieControllers(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        NollieController*     controller     = new NollieController(dev, info->path,info->product_id);
        RGBController_Nollie* rgb_controller = new RGBController_Nollie(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR("Nollie 32CH", DetectNollieControllers, NOLLIE32_VID, NOLLIE32_PID);
REGISTER_HID_DETECTOR("Nollie 16CH", DetectNollieControllers, NOLLIE16_VID, NOLLIE16_PID);
REGISTER_HID_DETECTOR("Nollie 8CH", DetectNollieControllers, NOLLIE8_VID, NOLLIE8_PID);
REGISTER_HID_DETECTOR("Nollie 1CH", DetectNollieControllers, NOLLIE1_VID, NOLLIE1_PID);
REGISTER_HID_DETECTOR("Nollie 28 12", DetectNollieControllers, NOLLIE28_12_VID, NOLLIE28_12_PID);
REGISTER_HID_DETECTOR("Nollie 28 L1", DetectNollieControllers, NOLLIE28_12_VID, NOLLIE28_L1_PID);
REGISTER_HID_DETECTOR("Nollie 28 L2", DetectNollieControllers, NOLLIE28_12_VID, NOLLIE28_L2_PID);
