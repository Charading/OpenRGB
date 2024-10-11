/*---------------------------------------------------------*\
| ViewSonicControllerDetect.cpp                             |
|                                                           |
|   Detector for ViewSonic XG270QG                          |
|                                                           |
|   Lanzaa                                      23 Jan 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "VS_XG270QG_Controller.h"
#include "RGBController_XG270QG.h"
#include "VS_XG270QG_Controller.h"
#include "RGBController_XG270QG.h"

#define VIEWSONIC_VID 0x0416
#define VS_XG270QG_PID 0x5020

GENERIC_HOTPLUGGABLE_DETECTOR(DetectViewSonic, VS_XG270QG_Controller, RGBController_XG270QG)

REGISTER_HID_DETECTOR_IPU("ViewSonic Monitor XG270QG", DetectViewSonic, VIEWSONIC_VID, VS_XG270QG_PID, 0, 0xFF00, 1);
