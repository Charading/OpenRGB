/*---------------------------------------------------------*\
| IntelArcA770LEControllerDetect.cpp                        |
|                                                           |
|   Detector for Intel Arc A770 LE                          |
|                                                           |
|   Adam Honse (CalcProgrammer1)                01 Nov 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "IntelArcA770LEController.h"
#include "RGBController_IntelArcA770LE.h"

#define INTEL_ARC_A770_LIMITED_EDITION_VID  0x2516
#define INTEL_ARC_A770_LIMITED_EDITION_PID  0x01B5

/******************************************************************************************\
*                                                                                          *
*   DetectIntelArcA770LEControllers                                                        *
*                                                                                          *
*       Tests the USB address to see if an Intel Arc A770 LE controller exists there.      *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectIntelArcA770LEControllers, IntelArcA770LEController, RGBController_IntelArcA770LE)

REGISTER_HID_DETECTOR_IP("Intel Arc A770 Limited Edition", DetectIntelArcA770LEControllers, INTEL_ARC_A770_LIMITED_EDITION_VID, INTEL_ARC_A770_LIMITED_EDITION_PID, 1, 0xFF00);
