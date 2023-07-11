/*---------------------------------------------------------*\
| LGMonitorControllerDetect.cpp                             |
|                                                           |
|   Detector for LG monitor                                 |
|                                                           |
|   Morgan Guimard (morg)                       11 Oct 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "LGMonitorController.h"
#include "RGBController.h"
#include "RGBController_LGMonitor.h"
#include "dmiinfo.h"

/*---------------------------------------------------------*\
| vendor ID                                                 |
\*---------------------------------------------------------*/
#define LG_MONITOR_VID                                 0x043E

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define LG_27GN950_B_PID                               0x9A8A
#define LG_38GL950G_PID                                0x9A57

static Controllers DetectLGMonitorControllers(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LGMonitorController*     controller         = new LGMonitorController(dev, *info);
        RGBController_LGMonitor* rgb_controller     = new RGBController_LGMonitor(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_HID_DETECTOR_IPU("LG 27GN950-B Monitor", DetectLGMonitorControllers, LG_MONITOR_VID, LG_27GN950_B_PID, 1, 0xFF01, 0x01);

// Untested
//REGISTER_HID_DETECTOR("LG 38GL950G Monitor",  DetectLGMonitorControllers, LG_MONITOR_VID, LG_38GL950G_PID);
