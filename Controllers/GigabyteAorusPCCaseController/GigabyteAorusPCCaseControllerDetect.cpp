/*---------------------------------------------------------*\
| GigabyteAorusPCCaseControllerDetect.cpp                   |
|                                                           |
|   Detector for Gigabyte Aorus case                        |
|                                                           |
|   Denis Nazarov (nenderus)                    10 Feb 2024 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "GigabyteAorusPCCaseController.h"
#include "RGBController_GigabyteAorusPCCase.h"

/*-----------------------------------------------------*\
| Vendor ID                                             |
\*-----------------------------------------------------*/
#define HOLTEK_VID                          0x1044

/*-----------------------------------------------------*\
| Controller product ids                                |
\*-----------------------------------------------------*/
#define C300_GLASS_PID                      0x7A30

/******************************************************************************************\
*                                                                                          *
*   DetectGigabyteAorusPCCaseControllers                                                   *
*                                                                                          *
*       Tests the USB address to see if a Gigabyte Aorus PC Case exists there.             *
*                                                                                          *
\******************************************************************************************/
static Controllers DetectGigabyteAorusPCCaseControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        GigabyteAorusPCCaseController*      controller      = new GigabyteAorusPCCaseController(dev, info->path);
        RGBController_GigabyteAorusPCCase*  rgb_controller  = new RGBController_GigabyteAorusPCCase(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("Gigabyte AORUS C300 GLASS",  DetectGigabyteAorusPCCaseControllers,   HOLTEK_VID, C300_GLASS_PID, 0,  0xFF01, 0x01);
