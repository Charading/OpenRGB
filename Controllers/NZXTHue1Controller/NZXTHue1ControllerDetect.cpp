/*---------------------------------------------------------*\
| NZXTHue1ControllerDetect.cpp                              |
|                                                           |
|   Detector for NZXT Hue 1 (Smart Device V1)               |
|                                                           |
|   Adam Honse (calcprogrammer1@gmail.com)      16 Apr 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "NZXTHue1Controller.h"
#include "RGBController_NZXTHue1.h"

/*-----------------------------------------------------*\
| NZXT USB IDs                                          |
\*-----------------------------------------------------*/
#define NZXT_VID                        0x1E71
#define NZXT_SMART_DEVICE_V1_PID        0x1714

/******************************************************************************************\
*                                                                                          *
*   DetectNZXTHue1Controllers                                                              *
*                                                                                          *
*       Detect devices supported by the NZXTHue1 driver                                    *
*                                                                                          *
\******************************************************************************************/

static Controllers DetectNZXTHue1Controllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        NZXTHue1Controller*     controller     = new NZXTHue1Controller(dev, 3, info->path);
        RGBController_NZXTHue1* rgb_controller = new RGBController_NZXTHue1(controller);
        result.push_back(rgb_controller);
    }
    return result;
}   /* DetectNZXTHue1Controllers() */

REGISTER_HID_DETECTOR("NZXT Smart Device V1",   DetectNZXTHue1Controllers,  NZXT_VID,   NZXT_SMART_DEVICE_V1_PID);
