/*-----------------------------------------*\
|  AMDWraithPrismController.h               |
|                                           |
|  Driver for AMD Wraith Prism RGB lighting |
|  controller                               |
|                                           |
|  Adam Honse (CalcProgrammer1) 12/6/2019   |
\*-----------------------------------------*/

#include "AMDWraithPrismController.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

AMDWraithPrismController::AMDWraithPrismController(libusb_device_handle* dev_handle)
{
    dev = dev_handle;

    strcpy(device_name, "AMD Wraith Prism");

    SendEnableCommand();
    SetRingEffectChannel(0x00);
}

AMDWraithPrismController::~AMDWraithPrismController()
{

}

char* AMDWraithPrismController::GetDeviceName()
{
    return device_name;
}

std::string AMDWraithPrismController::GetEffectChannelString(unsigned char channel)
{
    std::string ret_string = "";

    unsigned char usb_buf[] =
    {
        0x40, 0x21, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    int             actual;

    usb_buf[0x02] = channel;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);

    ret_string.append((char *)&usb_buf[0x08]);

    return(ret_string);
}

std::string AMDWraithPrismController::GetFirmwareVersionString()
{
    std::string ret_string = "";

    unsigned char usb_buf[] =
    {
        0x12, 0x20, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    int             actual;
    unsigned char   fw_buf[16] = {0x00};

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);

    for(int char_idx = 0; char_idx < 16; char_idx+=2)
    {
        if(usb_buf[char_idx + 0x08] != 0)
        {
            fw_buf[char_idx / 2] = usb_buf[char_idx + 0x08];
        }
        else
        {
            break;
        }
    }

    ret_string.append((char *)fw_buf);

    return(ret_string);
}

void AMDWraithPrismController::SetFanColor(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char usb_buf[] =
    {
        0x51, 0x2C, 0x01, 0x00,
        0x06, 0xFF, 0x00, 0x01,
        0xFF, 0xFF, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    int             actual;

    usb_buf[0x0A] = red;
    usb_buf[0x0B] = green;
    usb_buf[0x0C] = blue;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SetLogoColor(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char usb_buf[] =
    {
        0x51, 0x2C, 0x01, 0x00,
        0x05, 0xFF, 0x00, 0x01,
        0xFF, 0xFF, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    int             actual;

    usb_buf[0x0A] = red;
    usb_buf[0x0B] = green;
    usb_buf[0x0C] = blue;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SetRingColor(unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char usb_buf[] =
    {
        0x51, 0x2C, 0x01, 0x00,
        0x00, 0xFF, 0x00, 0xFF,
        0xFF, 0xFF, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    int             actual;

    usb_buf[0x0A] = red;
    usb_buf[0x0B] = green;
    usb_buf[0x0C] = blue;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SetRingEffectChannel(unsigned char channel)
{
    unsigned char usb_buf[] =
    {
        0x51, 0xA0, 0x01, 0x00,
        0x00, 0x03, 0x00, 0x00,
        0x05, 0x06, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    int             actual;

    for(int led = 0x0A; led <= 0x18; led++)
    {
        usb_buf[led] = channel;
    }

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SendEnableCommand()
{
    unsigned char usb_buf[] =
    {
        0x41, 0x80, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    int             actual;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SendRemapCommand()
{
    unsigned char usb_buf[] =
    {
        0x51, 0xA0, 0x01, 0x00,
        0x00, 0x03, 0x00, 0x00,
        0x05, 0x06, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x07, 0x07, 0x07,
        0x07, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };

    int             actual;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}

void AMDWraithPrismController::SendEffectCommand()
{
    unsigned char usb_buf[] =
    {
        0x51, 0x2C, 0x01, 0x00,
        0x05, 0xFF, 0x00, 0x01,
        0xFF, 0xFF, 0x00, 0xFF,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF
    };

    int             actual;

    libusb_interrupt_transfer(dev, 0x04, usb_buf, 64, &actual, 0);
    libusb_interrupt_transfer(dev, 0x83, usb_buf, 64, &actual, 0);
}
