#pragma once

#include <RGBController.h>
#include <libusb-1.0/libusb.h>

class uDMXController
{
public:

    unsigned char DMX_Buffer[512];

    uDMXController(libusb_device_handle * dev_handle);
    ~uDMXController();

    std::string GetDeviceName();

    void SendPacket();

private:
    libusb_device_handle* dev;
};
