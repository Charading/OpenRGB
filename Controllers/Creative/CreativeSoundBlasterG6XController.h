#include "RGBController.h"
#include <cstring>
#include <hidapi/hidapi.h>

#pragma once

class CreativeSoundBlasterG6XController
{
public:
    CreativeSoundBlasterG6XController(hid_device* dev_handle, const char* path);
    ~CreativeSoundBlasterG6XController();
    void            SetLedColor (unsigned char red, unsigned char green, unsigned char blue);

    std::string GetDeviceLocation();
    std::string GetSerialString();

private:
    hid_device*             dev;
    std::string             location;
};
