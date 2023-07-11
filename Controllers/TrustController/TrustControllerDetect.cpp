/*---------------------------------------------------------*\
| TrustControllerDetect.cpp                                 |
|                                                           |
|   Detector for Trust devices                              |
|                                                           |
|   Morgan Guimard (morg)                       24 Jan 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "TrustGXT114Controller.h"
#include "TrustGXT180Controller.h"
#include "RGBController_TrustGXT114.h"
#include "RGBController_TrustGXT180.h"

/*---------------------------------------------------------*\
| Trust vendor ID                                           |
\*---------------------------------------------------------*/
#define TRUST_VID                                      0x145F

/*---------------------------------------------------------*\
| Product IDs                                               |
\*---------------------------------------------------------*/
#define TRUST_GXT_114_PID                              0x026D
#define TRUST_GXT_180_PID                              0x0248

static Controllers DetectTrustGXT114Controllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        TrustGXT114Controller* controller               = new TrustGXT114Controller(dev, *info);
        if(controller->Test())
        {
            RGBController_TrustGXT114* rgb_controller   = new RGBController_TrustGXT114(controller);
            result.push_back(rgb_controller);
        }
        else
        {
            delete controller;
        }
    }
    return result;
}

static Controllers DetectTrustGXT180Controllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        TrustGXT180Controller* controller           = new TrustGXT180Controller(dev, *info);
        RGBController_TrustGXT180* rgb_controller   = new RGBController_TrustGXT180(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("Trust GXT 114", DetectTrustGXT114Controllers, TRUST_VID, TRUST_GXT_114_PID, 1, 0xFF00, 1);
REGISTER_HID_DETECTOR_IPU("Trust GXT 180", DetectTrustGXT180Controllers, TRUST_VID, TRUST_GXT_180_PID, 1, 0xFFA0, 1);
