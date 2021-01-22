/*-----------------------------------------*\
|  AsusAuraCoreController.cpp               | 
|                                           |
|  Driver for ASUS ROG Aura Core RGB        |
|  lighting controller                      |
|                                           |
|  Adam Honse (CalcProgrammer1) 4/13/2020   |
\*-----------------------------------------*/

#include "AsusAuraCoreController.h"
#include <cstring>



AuraCoreController::AuraCoreController(hid_device* dev_handle, const char* path):
    aura_device()
{
    dev         = dev_handle;
    location    = path;

    IdentifyDevice();
}

AuraCoreController::~AuraCoreController()
{
}

std::string AuraCoreController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string AuraCoreController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void AuraCoreController::SendBrightness
    (
    unsigned char   brightness
    )
{
    unsigned char usb_buf[aura_device.buff_size];


    if(aura_device.aura_type != AURA_CORE_DEVICE_UNKNOWN)
    {
        /*-----------------------------------------------------*\
        | Zero out buffer                                       |
        \*-----------------------------------------------------*/
        memset(usb_buf, 0x00, sizeof(usb_buf));

        /*-----------------------------------------------------*\
        | Set up message packet                                 |
        \*-----------------------------------------------------*/
        usb_buf[0x00]   = 0x5A;
        usb_buf[0x01]   = AURA_CORE_COMMAND_BRIGHTNESS;
        usb_buf[0x02]   = 0xC5;
        usb_buf[0x03]   = 0xC4;
        usb_buf[0x04]   = brightness;

        /*-----------------------------------------------------*\
        | Send packet                                           |
        \*-----------------------------------------------------*/
        hid_send_feature_report(dev, usb_buf, sizeof(usb_buf));
    }
}

void AuraCoreController::SendUpdate
    (
    unsigned char   zone,
    unsigned char   mode,
    unsigned char   speed,
    unsigned char   dir,
    unsigned char   red,
    unsigned char   green,
    unsigned char   blue
    )
{
    unsigned char usb_buf[aura_device.buff_size];


    if(aura_device.aura_type != AURA_CORE_DEVICE_UNKNOWN)
    {
        /*-----------------------------------------------------*\
        | Zero out buffer                                       |
        \*-----------------------------------------------------*/
        memset(usb_buf, 0x00, sizeof(usb_buf));

        /*-----------------------------------------------------*\
        | Set up message packet                                 |
        \*-----------------------------------------------------*/
        usb_buf[0x00]   = aura_device.report_id;
        usb_buf[0x01]   = AURA_CORE_COMMAND_UPDATE;
        usb_buf[0x02]   = zone;
        usb_buf[0x03]   = mode;
        usb_buf[0x04]   = red;
        usb_buf[0x05]   = green;
        usb_buf[0x06]   = blue;
        usb_buf[0x07]   = speed;
        usb_buf[0x08]   = dir;

        /*-----------------------------------------------------*\
        | Send packet                                           |
        \*-----------------------------------------------------*/
        hid_send_feature_report(dev, usb_buf, sizeof(usb_buf));
    }
}

void AuraCoreController::SendSet()
{
    unsigned char usb_buf[aura_device.buff_size];


    if(aura_device.aura_type != AURA_CORE_DEVICE_UNKNOWN)
    {
        /*-----------------------------------------------------*\
        | Zero out buffer                                       |
        \*-----------------------------------------------------*/
        memset(usb_buf, 0x00, sizeof(usb_buf));

        /*-----------------------------------------------------*\
        | Set up message packet                                 |
        \*-----------------------------------------------------*/
        usb_buf[0x00]   = aura_device.report_id;
        usb_buf[0x01]   = AURA_CORE_COMMAND_SET;

        /*-----------------------------------------------------*\
        | Send packet                                           |
        \*-----------------------------------------------------*/
        hid_send_feature_report(dev, usb_buf, sizeof(usb_buf));
    }
}

void AuraCoreController::SendApply()
{
    unsigned char usb_buf[aura_device.buff_size];


    if(aura_device.aura_type != AURA_CORE_DEVICE_UNKNOWN)
    {
        /*-----------------------------------------------------*\
        | Zero out buffer                                       |
        \*-----------------------------------------------------*/
        memset(usb_buf, 0x00, sizeof(usb_buf));

        /*-----------------------------------------------------*\
        | Set up message packet                                 |
        \*-----------------------------------------------------*/
        usb_buf[0x00]   = aura_device.report_id;
        usb_buf[0x01]   = AURA_CORE_COMMAND_APPLY;

        /*-----------------------------------------------------*\
        | Send packet                                           |
        \*-----------------------------------------------------*/
        hid_send_feature_report(dev, usb_buf, sizeof(usb_buf));
    }
}

void AuraCoreController::IdentifyDevice()
{
    unsigned char   usb_buf[64];
    int             num_bytes       = 0;

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(usb_buf, 0x00, sizeof(usb_buf));

    // First, attempt to read the report from the Keyboard
    usb_buf[0x00]   = aura_device.report_id;

    num_bytes = hid_get_feature_report(dev, usb_buf, sizeof(usb_buf));

    if(num_bytes > 0)
    {
        // Currently, there is no need to attempt to read the returned data
        // If the device responded,to the report, we're good.
        aura_device.aura_type = AURA_CORE_DEVICE_KEYBOARD;
        aura_device.buff_size = 17;
        aura_device.report_id = 0x5D;
    }
    else if(num_bytes == -1)
    {
        // First, attempt to read the report from the Keyboard
        usb_buf[0] = 0x5E;
        num_bytes = hid_get_feature_report(dev, usb_buf, sizeof(usb_buf));

        if(num_bytes > 0)
        {
            // Currently, there is no need to attempt to read the returned data
            // If the device responded,to the report, we're good.
            aura_device.aura_type = AURA_CORE_DEVICE_GA15DH;
            aura_device.buff_size = 64;
            aura_device.report_id = 0x5E;
        }
    }
}
