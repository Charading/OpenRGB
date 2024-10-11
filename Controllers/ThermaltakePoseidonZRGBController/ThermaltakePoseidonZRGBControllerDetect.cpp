/*---------------------------------------------------------*\
| ThermaltakePoseidonZRGBControllerDetect.cpp               |
|                                                           |
|   Detector for Thermaltake Poseidon Z RGB                 |
|                                                           |
|   Adam Honse (CalcProgrammer1)                25 Dec 2019 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "ThermaltakePoseidonZRGBController.h"
#include "RGBController_ThermaltakePoseidonZRGB.h"

#define TT_POSEIDON_Z_RGB_VID 0x264A
#define TT_POSEIDON_Z_RGB_PID 0x3006

/******************************************************************************************\
*                                                                                          *
*   DetectPoseidonZRGBControllers                                                          *
*                                                                                          *
*       Tests the USB address to see if a Thermaltake Poseidon Z RGB Keyboard controller   *
*       exists there.                                                                      *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectPoseidonZRGBControllers, PoseidonZRGBController, RGBController_PoseidonZRGB)

REGISTER_HID_DETECTOR_IP("Thermaltake Poseidon Z RGB", DetectPoseidonZRGBControllers, TT_POSEIDON_Z_RGB_VID, TT_POSEIDON_Z_RGB_PID, 1, 0xFF01);
