#include "CreativeSoundBlasterG6XController.h"

CreativeSoundBlasterG6XController::CreativeSoundBlasterG6XController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;
}

CreativeSoundBlasterG6XController::~CreativeSoundBlasterG6XController()
{
    hid_close(dev);
}

std::string CreativeSoundBlasterG6XController::GetDeviceLocation()
{
    return("HID " + location);
}

void CreativeSoundBlasterG6XController::SetLedColor (unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char usb_buf[64];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    //5A 3A 02 06 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    usb_buf[0x01]           = 0x5A;
    usb_buf[0x02]           = 0x3A;
    usb_buf[0x03]           = 0x02;
    usb_buf[0x04]           = 0x06;
    usb_buf[0x05]           = 0x01;
    //hid_send_feature_report(dev, usb_buf, 64);
    hid_write(dev, (unsigned char *)usb_buf, 64);

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    //5A 3A 06 04 00 03 01 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
    usb_buf[0x01]           = 0x5A;
    usb_buf[0x02]           = 0x3A;
    usb_buf[0x03]           = 0x06;
    usb_buf[0x04]           = 0x04;
    usb_buf[0x06]           = 0x03;
    usb_buf[0x07]           = 0x01;
    usb_buf[0x09]           = 0x01;
    //hid_send_feature_report(dev, usb_buf, 64);
    hid_write(dev, (unsigned char *)usb_buf, 64);


    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    //5A 3A 09 0A 00 03 01 01 FF BB GG RR
    usb_buf[0x01]           = 0x5A;
    usb_buf[0x02]           = 0x3A;
    usb_buf[0x03]           = 0x09;
    usb_buf[0x04]           = 0x0A;
    usb_buf[0x06]           = 0x03;
    usb_buf[0x07]           = 0x01;
    usb_buf[0x08]           = 0x01;
    usb_buf[0x09]           = 0x0FF;

    usb_buf[0x0A]           = blue;
    usb_buf[0x0B]           = green;
    usb_buf[0x0C]           = red;
    //hid_send_feature_report(dev, usb_buf, 64);
    hid_write(dev, (unsigned char *)usb_buf, 64);
    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    //hid_write(dev, (unsigned char *)usb_buf, 20);
    //hid_read(dev, (unsigned char *)usb_buf, 20);

}
