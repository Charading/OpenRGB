#pragma once
#include "RGBController.h"

#include <string>
#include <vector>
#include <hidapi/hidapi.h>

#pragma once

class AnnePro2Controller
{
public:
    AnnePro2Controller(hid_device* dev_handle, const char* path);
    virtual ~AnnePro2Controller();

    std::string GetDeviceLocation();
    std::string GetSerialString();

    void SendDirect
        (
        unsigned char       frame_count,
        unsigned char *     frame_data
        );

private:
    hid_device*                 dev;
    std::string                 location;
};