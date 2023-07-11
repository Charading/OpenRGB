/*---------------------------------------------------------*\
| AOCMouseControllerDetect.cpp                              |
|                                                           |
|   Detector for AOC mouse                                  |
|                                                           |
|   Adam Honse (CalcProgrammer1)                20 Apr 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AOCMouseController.h"
#include "RGBController_AOCMouse.h"

/*-----------------------------------------------------*\
| AOC Mouse IDs                                         |
\*-----------------------------------------------------*/
#define AOC_VID       0x3938
#define AOC_GM500_PID 0x1179

/******************************************************************************************\
*                                                                                          *
*   DetectAOCMouseControllers                                                              *
*                                                                                          *
*       Tests the USB address to see if an AOC Mouse controller exists there.              *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAOCMouseControllers, AOCMouseController, RGBController_AOCMouse)

REGISTER_HID_DETECTOR_IPU("AOC GM500", DetectAOCMouseControllers, AOC_VID, AOC_GM500_PID, 1, 0xFF19, 0xFF19);
