/*---------------------------------------------------------*\
| CreativeSoundBlasterXG6Controller.h                       |
|                                                           |
|   Driver for Creative SoundBlaster XG6                    |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include "RGBController.h"
#include "hidapi_wrapper.h"

#include <cstring>

class CreativeSoundBlasterXG6Controller
{
public:
    CreativeSoundBlasterXG6Controller(hid_device* dev_handle, const char* path);
    ~CreativeSoundBlasterXG6Controller();

    void                    SetLedColor(unsigned char red,
                                        unsigned char green,
                                        unsigned char blue,
                                        unsigned char brightness);

    std::string             GetDeviceLocation();
    std::string             GetSerialString();

private:
    hid_device*             dev;
    std::string             location;
};
