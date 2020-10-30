/*-----------------------------------------*\
|  LogitechG560Controller.cpp               |
|                                           |
|  Driver for Logitech G560 RGB Speaker     |
|  Charging System                          |
|                                           |
|  Cheerpipe     10/28/2020                 |
|  Based on                                 |
|  TheRogueZeta   8/31/2020                 |
\*-----------------------------------------*/

#include "LogitechG560Controller.h"

#include <cstring>

LogitechG560Controller::LogitechG560Controller(hid_device* dev_handle)
{
    dev = dev_handle;
}

LogitechG560Controller::~LogitechG560Controller()
{
    hid_close(dev);
}

void LogitechG560Controller::SendSpeakerMode
    (
    unsigned char       zone,           //0x04
    unsigned char       mode,           //0x05
    unsigned char       red,            //0x06
    unsigned char       green,          //0x07
    unsigned char       blue            //0x08
    )
{
    unsigned char usb_buf[20];
    unsigned char usb_buf_out[33];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));
     memset(usb_buf_out, 0x00, sizeof(usb_buf_out));

    /*-----------------------------------------------------*\
    | Set up Lighting Control packet                        |
    \*-----------------------------------------------------*/
    usb_buf[0x00]           = 0x11;
    usb_buf[0x01]           = 0xFF;
    usb_buf[0x02]           = 0x04;
    usb_buf[0x03]           = 0x3C;

    usb_buf[0x04]           = zone;
    usb_buf[0x05]           = mode;

    usb_buf[0x06]           = red;
    usb_buf[0x07]           = green;
    usb_buf[0x08]           = blue;

    if(mode == LOGITECH_G560_MODE_STATIC)
    {
        usb_buf[0x09]   = 0x02;
    }
    if(mode == LOGITECH_G560_MODE_CYCLE)
    {
        usb_buf[0x0B]   = speed >> 8;
        usb_buf[0x0C]   = speed & 0xFF;
        //usb_buf[0x0D]   = brightness;
        usb_buf[0x0D]   = 0x64;
    }
    else if(mode == LOGITECH_G560_MODE_BREATHING)
    {
        usb_buf[0x09]   = speed >> 8;
        usb_buf[0x0A]   = speed & 0xFF;
        //usb_buf[0x0C]   = brightness;
        usb_buf[0x0C]   = 0x64;
    }
    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, usb_buf, 20);
    hid_read(dev, usb_buf_out, 33);
}
