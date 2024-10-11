/*---------------------------------------------------------*\
| GigabyteAorusCPUCoolerControllerDetect.cpp                |
|                                                           |
|   Detector for Gigabyte Aorus CPU coolers                 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "RGBController.h"
#include "ATC800Controller.h"
#include "RGBController_AorusATC800.h"

/*-----------------------------------------------------*\
| Vendor ID                                             |
\*-----------------------------------------------------*/
#define HOLTEK_VID                            0x1044

/*-----------------------------------------------------*\
| Controller product ids                                |
\*-----------------------------------------------------*/
#define ATC_800_CONTROLLER_PID                0x7A42

/******************************************************************************************\
*                                                                                          *
*   DetectAorusCPUCoolerControllers                                                        *
*                                                                                          *
*       Tests the USB address to see if a Aorus RGB CPU Cooler exists there.               *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectGigabyteAorusCPUCoolerControllers, ATC800Controller, RGBController_AorusATC800)

REGISTER_HID_DETECTOR_IPU("Gigabyte AORUS ATC800", DetectGigabyteAorusCPUCoolerControllers, HOLTEK_VID, ATC_800_CONTROLLER_PID, 0, 0xFF01, 1);
