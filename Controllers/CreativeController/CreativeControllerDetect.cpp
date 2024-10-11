/*---------------------------------------------------------*\
| CreativeControllerDetect.cpp                              |
|                                                           |
|   Detector for Creative devices                           |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "CreativeSoundBlasterXG6Controller.h"
#include "RGBController_CreativeSoundBlasterXG6.h"
#include "HidDetector.h"

/*-----------------------------------------------------*\
| Creative vendor ID                                    |
\*-----------------------------------------------------*/
#define CREATIVE_VID                            0x041E
/*-----------------------------------------------------*\
| SoundCards                                            |
\*-----------------------------------------------------*/
#define CREATIVE_SOUNDBLASTERX_G6_PID           0x3256

GENERIC_HOTPLUGGABLE_DETECTOR(DetectCreativeDevice, CreativeSoundBlasterXG6Controller,RGBController_CreativeSoundBlasterXG6)

/*-------------------------------------------------------------------------------------------------------------------------------------------------*\
| Sound Cards                                                                                                                                       |
\*-------------------------------------------------------------------------------------------------------------------------------------------------*/
REGISTER_HID_DETECTOR_I("Creative SoundBlasterX G6", DetectCreativeDevice, CREATIVE_VID, CREATIVE_SOUNDBLASTERX_G6_PID, 4);
