#include "RGBController_DMX.h"
#include <uDMXController.h>

uDMXController::uDMXController(libusb_device_handle * dev_handle)
{
    dev = dev_handle;
}

uDMXController::~uDMXController()
{
    libusb_close(dev);
}

std::string uDMXController::GetDeviceName()
{
    char string[128];

    libusb_get_string_descriptor_ascii(dev, 2, (unsigned char *)string, 128);

    return(std::string(string));
}

void uDMXController::SendPacket()
{
    /*-----------------------------------------------------------------*\
    | Issue: uDMX has a low refresh rate and is a USB Low-Speed device. |
    | When a packet is sended, sometimes the device does not refresh    |
    | properly and keeps the last DMX frame and the lights remains on.  |
    | This is more notable when you adjust the brightness of the lights |
    \*-----------------------------------------------------------------*/

    libusb_control_transfer(dev, 0x40, 2, 0x0200, 0, DMX_Buffer, 512, 1);
}
