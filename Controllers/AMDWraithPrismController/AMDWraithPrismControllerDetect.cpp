/*---------------------------------------------------------*\
| AMDWraithPrismControllerDetect.cpp                        |
|                                                           |
|   Detector for AMD Wraith Prism                           |
|                                                           |
|   Adam Honse (CalcProgrammer1)                06 Dec 2019 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AMDWraithPrismController.h"
#include "RGBController_AMDWraithPrism.h"

/*---------------------------------------------------------*\
| AMD Wraith Prism vendor ID                                |
\*---------------------------------------------------------*/
#define AMD_WRAITH_PRISM_VID                        0x2516

/*---------------------------------------------------------*\
| AMD Wraith Prism product ID                               |
\*---------------------------------------------------------*/
#define AMD_WRAITH_PRISM_PID                        0x0051

/******************************************************************************************\
*                                                                                          *
*   DetectAMDWraithPrismControllers                                                        *
*                                                                                          *
*       Tests the USB address to see if an AMD Wraith Prism controller exists there.       *
*                                                                                          *
\******************************************************************************************/

static Controllers DetectAMDWraithPrismControllers(hid_device_info* info, const std::string&)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if( dev )
    {
        AMDWraithPrismController*     controller     = new AMDWraithPrismController(dev, info->path);
        RGBController_AMDWraithPrism* rgb_controller = new RGBController_AMDWraithPrism(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IP("AMD Wraith Prism", DetectAMDWraithPrismControllers, AMD_WRAITH_PRISM_VID, AMD_WRAITH_PRISM_PID, 1, 0xFF00);
