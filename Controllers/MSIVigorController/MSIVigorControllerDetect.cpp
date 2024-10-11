/*---------------------------------------------------------*\
| MSIVigorControllerDetect.cpp                              |
|                                                           |
|   Detector for MSI Vigor                                  |
|                                                           |
|   Morgan Guimard (morg)                       01 Jun 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "MSIVigorGK30Controller.h"
#include "RGBController_MSIVigorGK30.h"

/*---------------------------------------------------------*\
| MSI vendor ID                                             |
\*---------------------------------------------------------*/
#define MSI_VID                                        0x0DB0

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define MSI_VIGOR_GK30_PID                             0x0B30

static Controllers DetectMSIVigorGK30Controllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        MSIVigorGK30Controller*     controller      = new MSIVigorGK30Controller(dev, *info);
        RGBController_MSIVigorGK30* rgb_controller  = new RGBController_MSIVigorGK30(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("MSI Vigor GK30 controller", DetectMSIVigorGK30Controllers, MSI_VID, MSI_VIGOR_GK30_PID, 1, 0xFF01, 1);
