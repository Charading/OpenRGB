/*---------------------------------------------------------*\
| AOCMousematControllerDetect.cpp                           |
|                                                           |
|   Detector for AOC mousemat                               |
|                                                           |
|   Adam Honse (CalcProgrammer1)                15 Apr 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AOCMousematController.h"
#include "RGBController_AOCMousemat.h"

/*-----------------------------------------------------*\
| AOC Mousemat IDs                                      |
\*-----------------------------------------------------*/
#define AOC_VID        0x3938
#define AOC_AMM700_PID 0x1162

/******************************************************************************************\
*                                                                                          *
*   DetectAOCMousematControllers                                                           *
*                                                                                          *
*       Tests the USB address to see if an AOC Mousemat controller exists there.           *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAOCMousematControllers, AOCMousematController, RGBController_AOCMousemat)

REGISTER_HID_DETECTOR_IPU("AOC AGON AMM700",    DetectAOCMousematControllers,   AOC_VID,    AOC_AMM700_PID, 1,  0xFF19,  0xFF19);
