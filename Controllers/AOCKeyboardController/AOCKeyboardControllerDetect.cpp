/*---------------------------------------------------------*\
| AOCKeyboardControllerDetect.cpp                           |
|                                                           |
|   Detector for AOC keyboard                               |
|                                                           |
|   Adam Honse (CalcProgrammer1)                10 May 2023 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "AOCKeyboardController.h"
#include "RGBController_AOCKeyboard.h"

/*-----------------------------------------------------*\
| AOC Keyboard IDs                                      |
\*-----------------------------------------------------*/
#define AOC_VID                                     0x3938
#define AOC_GK500_PID                               0x1178
#define AOC_GK500_PID_2                             0x1229

/******************************************************************************************\
*                                                                                          *
*   DetectAOCKeyboardControllers                                                           *
*                                                                                          *
*       Tests the USB address to see if an AOC Keyboard controller exists there.           *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectAOCKeyboardControllers, AOCKeyboardController, RGBController_AOCKeyboard)

REGISTER_HID_DETECTOR_PU("AOC GK500",  DetectAOCKeyboardControllers,   AOC_VID,    AOC_GK500_PID,   0xFF19, 0xFF19);
REGISTER_HID_DETECTOR_PU("AOC GK500",  DetectAOCKeyboardControllers,   AOC_VID,    AOC_GK500_PID_2, 0xFF19, 0xFF19);
