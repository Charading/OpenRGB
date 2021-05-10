/*-----------------------------------------*\
|  LogitechG203Controller.cpp               |
|                                           |
|  Driver for Logitech G203 Prodigy mouse   |
|  lighting controller                      |
|                                           |
|  Adam Honse (CalcProgrammer1) 5/17/2020   |
\*-----------------------------------------*/

#include "LogitechG203Controller.h"

#include <cstring>

LogitechG203Controller::LogitechG203Controller(hid_device* dev_cmd_handle, hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    cmd_dev     = dev_cmd_handle;
    location    = path;
}

LogitechG203Controller::~LogitechG203Controller()
{
    hid_close(dev);
    hid_close(cmd_dev);
}

std::string LogitechG203Controller::GetDeviceLocation()
{
    return("HID " + location);
}

std::string LogitechG203Controller::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

/*-------------------------------------------------------------------------------------------------*\
| Private packet sending functions.                                                                 |
\*-------------------------------------------------------------------------------------------------*/

void LogitechG203Controller::UpdateMouseLED
    (
    unsigned char       mode,
    unsigned short      speed,
    unsigned char       red,
    unsigned char       green,
    unsigned char       blue
    )
{

    char usb_buf[20];

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    /*-----------------------------------------------------*\
    | Set up Lighting Control packet                        |
    \*-----------------------------------------------------*/
    usb_buf[0x00]           = 0x11;
    usb_buf[0x01]           = 0xFF;
    usb_buf[0x02]           = 0x0E;
    usb_buf[0x03]           = 0x3C;
    usb_buf[0x04]           = 0x00;

    usb_buf[0x05]           = mode;

    usb_buf[0x06]           = red;
    usb_buf[0x07]           = green;
    usb_buf[0x08]           = blue;

    speed = 100 * speed;
    if(mode == LOGITECH_G203_MODE_CYCLE)
    {
        usb_buf[0x0B]       = speed >> 8;
        usb_buf[0x0C]       = speed & 0xFF;
        usb_buf[0x0D]       = 0x64;
    }
    else if(mode == LOGITECH_G203_MODE_BREATHING)
    {
        usb_buf[0x09]       = speed >> 8;
        usb_buf[0x0A]       = speed & 0xFF;
        usb_buf[0x0C]       = 0x64;
    }

    /*-----------------------------------------------------*\
    | Send packet                                           |
    \*-----------------------------------------------------*/
    hid_write(dev, (unsigned char *)usb_buf, 20);
    hid_read(dev, (unsigned char *)usb_buf, 20);
}

void LogitechG203Controller::SendMouseMode(unsigned char mode)
{
    if(mode == LOGITECH_G203_MODE_DIRECT)
    {
        /*---------------------------------------------------------*\
        | Send packet to ensure device will not save to flash       |
        \*---------------------------------------------------------*/
        char cmd_buf[7];
        char usb_buf[20];
        /*-----------------------------------------------------*\
        | Zero out buffer                                       |
        \*-----------------------------------------------------*/
        memset(cmd_buf, 0x00, sizeof(cmd_buf));

        /*-----------------------------------------------------*\
        | Set up Command Control packet                         |
        \*-----------------------------------------------------*/
        cmd_buf[0x00]       = 0x10;
        cmd_buf[0x01]       = 0xFF;
        cmd_buf[0x02]       = 0x0E;
        cmd_buf[0x03]       = 0x8A;
        cmd_buf[0x04]       = 0x00;
        cmd_buf[0x05]       = 0x00;
        /*---------------------------------------------------------*\
        | Set 0x04, 0x05 to 1 to disable save to flash, 0 to enable |
        \*---------------------------------------------------------*/
        cmd_buf[0x04]       = 0x01;
        cmd_buf[0x05]       = 0x01;
        /*-----------------------------------------------------*\
        | Send packet                                           |
        \*-----------------------------------------------------*/
        hid_write(cmd_dev, (unsigned char *)cmd_buf, 7);
        hid_read(dev, (unsigned char *)usb_buf, 20);
    }
}
