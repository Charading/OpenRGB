/*-----------------------------------------*\
|  MSICoreliquidController.cpp              |
|                                           |
|  MSI Coreliquid lighting controller       |
|                                           |
|  Aapo Kossi 25/1/2023                     |
\*-----------------------------------------*/

#include "MSICoreliquidController.h"
#include <string.h>

MSICoreliquidController::MSICoreliquidController(hid_device* dev_handle, const hid_device_info& info)
{
    dev      = dev_handle;
    location = info.path;
    version  = "";

    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        serial_number = "";
    }
    else
    {
        std::wstring return_wstring = serial_string;
        serial_number = std::string(return_wstring.begin(), return_wstring.end());
    }
}

MSICoreliquidController::~MSICoreliquidController()
{
    hid_close(dev);
}

std::string MSICoreliquidController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string MSICoreliquidController::GetSerialString()
{
    return(serial_number);
}

std::string MSICoreliquidController::GetFirmwareVersion()
{
    return(version);
}

/*-----------------------------------------*\
| this is where the magic happens           |
\*-----------------------------------------*/
void MSICoreliquidController::SendColors
    (
        RGBColor color1,
        RGBColor color2,
        unsigned char mode,
        unsigned char speed,
        unsigned char brightness,
        unsigned char color_enabled
    )
{
 
    /*-----------------------------------------*\
    | initialize transfer buffer                |
    \*-----------------------------------------*/
    unsigned char usb_buf[CORELIQUID_REPORT_SIZE];
    memset(usb_buf, 0x00, CORELIQUID_REPORT_SIZE);

    /*-----------------------------------------*\
    | Set report ID and other constant bytes    |
    \*-----------------------------------------*/
    usb_buf[0x00] = CORELIQUID_REPORT_ID;
    usb_buf[0x29] = 0x08;
    usb_buf[0x3E] = 0xFC;
    usb_buf[0xB8] = 0x01;

    /*-----------------------------------------*\
    | Set primary colors                        |
    \*-----------------------------------------*/
    usb_buf[0x20] = RGBGetRValue(color1);
    usb_buf[0x21] = RGBGetGValue(color1);
    usb_buf[0x22] = RGBGetBValue(color1);

    /*-----------------------------------------*\
    | Set secondary colors                      |
    \*-----------------------------------------*/
    usb_buf[0x24] = RGBGetRValue(color2);
    usb_buf[0x25] = RGBGetGValue(color2);
    usb_buf[0x26] = RGBGetBValue(color2);

    /*-----------------------------------------*\
    | Set mode and its settings                 |
    \*-----------------------------------------*/
    usb_buf[0x1F] = mode;
    usb_buf[0x23] = speed + brightness;
    usb_buf[0x27] = color_enabled;

    /*-----------------------------------------*\
    | Send request to device                    |
    \*-----------------------------------------*/
    hid_send_feature_report(dev, usb_buf, CORELIQUID_REPORT_SIZE);
}

