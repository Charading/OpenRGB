/*-----------------------------------------*\
|  CorsairVoidProController.cpp             |
|                                           |
|  Driver for Corsair Void Pro lighting     |
|  controller                               |
|                                           |
|  Guimard Morgan (morg) 2/08/2022          |
\*-----------------------------------------*/
#include "CorsairVoidProController.h"
#include <string.h>

using namespace std::chrono_literals;

CorsairVoidProController::CorsairVoidProController(hid_device* dev_handle_light_control, hid_device* dev_handle_light_data, const hid_device_info& info)
{
    dev_light_control   = dev_handle_light_control;
    dev_light_data      = dev_handle_light_data;
    location            = info.path;
    version             = "";

    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev_handle_light_control, serial_string, 128);

    if(ret != 0)
    {
        serial_number = "";
    }
    else
    {
        std::wstring return_wstring = serial_string;
        serial_number = std::string(return_wstring.begin(), return_wstring.end());
    }

    /*-----------------------------------------*\
    |  Needs to be turned off before we send    |
    |  commands. Drawback : it turns the MIC    |
    |  LED off also.                            |
    \*-----------------------------------------*/
    SendState(false);
}

CorsairVoidProController::~CorsairVoidProController()
{
    hid_close(dev_light_control);
    hid_close(dev_light_data);
}

std::string CorsairVoidProController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string CorsairVoidProController::GetSerialString()
{
    return(serial_number);
}

std::string CorsairVoidProController::GetFirmwareVersion()
{
    return(version);
}

void CorsairVoidProController::SendState(bool on)
{
    unsigned char state = on ? 0x01 : 0x00;

    unsigned char usb_buf[3] =
    {
        0xC8, state, 0x00
    };

    hid_write(dev_light_control, usb_buf, 3);
}

void CorsairVoidProController::SetDirect(std::vector<RGBColor> colors)
{
    unsigned char usb_buf[CORSAIR_VOID_PRO_PACKET_SIZE] = {};

    memset(usb_buf, 0x00, CORSAIR_VOID_PRO_PACKET_SIZE);

    usb_buf[0]  = CORSAIR_VOID_PRO_REPORT_ID;
    usb_buf[1]  = DIRECT_MODE_VALUE;

    usb_buf[2]  = DIRECT_MODE_SEPARATOR_R1;
    usb_buf[3]  = RGBGetRValue(colors[0]);
    usb_buf[4]  = DIRECT_MODE_SEPARATOR_G1;
    usb_buf[5]  = RGBGetGValue(colors[0]);
    usb_buf[6]  = DIRECT_MODE_SEPARATOR_B1;
    usb_buf[7]  = RGBGetBValue(colors[0]);

    usb_buf[8]  = DIRECT_MODE_SEPARATOR_R2;
    usb_buf[9]  = RGBGetRValue(colors[1]);
    usb_buf[10] = DIRECT_MODE_SEPARATOR_G2;
    usb_buf[11] = RGBGetGValue(colors[1]);
    usb_buf[12] = DIRECT_MODE_SEPARATOR_B2;
    usb_buf[13] = RGBGetBValue(colors[1]);

    hid_write(dev_light_data, usb_buf, CORSAIR_VOID_PRO_PACKET_SIZE);
}
