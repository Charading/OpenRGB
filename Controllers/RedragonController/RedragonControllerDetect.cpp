/*---------------------------------------------------------*\
| RedragonControllerDetect.cpp                              |
|                                                           |
|   Detector for Redragon devices                           |
|                                                           |
|   Adam Honse (CalcProgrammer1)                15 Mar 2020 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "RedragonMouseController.h"
#include "RGBController_RedragonMouse.h"

/*-----------------------------------------------------*\
| Mouse product IDs                                     |
\*-----------------------------------------------------*/
#define REDRAGON_MOUSE_VID              0x04D9
#define REDRAGON_MOUSE_USAGE_PAGE       0xFFA0
#define REDRAGON_M711_PID               0xFC30
#define REDRAGON_M715_PID               0xFC39
#define REDRAGON_M716_PID               0xFC3A
#define REDRAGON_M908_PID               0xFC4D
#define REDRAGON_M602_PID               0xFC38
#define REDRAGON_M808_PID               0xFC5F
#define REDRAGON_M801_PID               0xFC58

/******************************************************************************************\
*                                                                                          *
*   DetectRedragonMice                                                                     *
*                                                                                          *
*       Tests the USB address to see if a Redragon Mouse controller exists there.          *
*                                                                                          *
\******************************************************************************************/

GENERIC_HOTPLUGGABLE_DETECTOR(DetectRedragonMice, RedragonMouseController, RGBController_RedragonMouse)

/*---------------------------------------------------------------------------------------------------------------------------------------------*\
| Mice                                                                                                                                          |
\*---------------------------------------------------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_IP("Redragon M711 Cobra",      DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M711_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M715 Dagger",     DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M715_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M716 Inquisitor", DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M716_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M908 Impact",     DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M908_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M602 Griffin",    DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M602_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M808 Storm",      DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M808_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
REGISTER_HID_DETECTOR_IP("Redragon M801 Sniper",     DetectRedragonMice,      REDRAGON_MOUSE_VID,    REDRAGON_M801_PID,         2, REDRAGON_MOUSE_USAGE_PAGE);
