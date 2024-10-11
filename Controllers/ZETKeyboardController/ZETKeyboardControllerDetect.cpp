/*---------------------------------------------------------*\
| ZETKeyboardControllerDetect.cpp                           |
|                                                           |
|   Detector for ZET Blade                                  |
|                                                           |
|   Based on HyperX Alloy Elite2 implementation by          |
|   KundaPanda                                              |
|                                                           |
|   Moon_darker (Vaker)                         23 Jan 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "ZETBladeOpticalController.h"
#include "RGBController_ZETBladeOptical.h"

/*-----------------------------------------------------*\
| ZET keyboard VID/PID pairs                            |
\*-----------------------------------------------------*/
#define ZET_BLADE_OPTICAL_VID                     0x2EA8
#define ZET_BLADE_OPTICAL_PID                     0x2125

GENERIC_HOTPLUGGABLE_DETECTOR(DetectZETBladeOptical, ZETBladeOpticalController, RGBController_ZETBladeOptical)

REGISTER_HID_DETECTOR_IP("ZET Blade Optical",    DetectZETBladeOptical, ZET_BLADE_OPTICAL_VID, ZET_BLADE_OPTICAL_PID,   1, 0xFF00);
