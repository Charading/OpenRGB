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

CorsairVoidProController::CorsairVoidProController(hid_device* dev_handle, const hid_device_info& info)
{
    dev                 = dev_handle;
    location            = info.path;
    version             = "";

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

    /*-----------------------------------------*\
    |  Needs to be turned off before we send    |
    |  commands. Drawback : it turns the MIC    |
    |  LED off also.                            |
    \*-----------------------------------------*/
    SetLightsState(LIGHTS_OFF);

    last_direct_mode_packet_time = std::chrono::steady_clock::now();
}

CorsairVoidProController::~CorsairVoidProController()
{
    hid_close(dev);
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

void CorsairVoidProController::SetLightsState(unsigned char state)
{
    unsigned char usb_buf[3] =
    {
        0xC8, state, 0x00
    };

    for(unsigned int i = 0; i < 3; i++)
    {
        printf("%02X ", usb_buf[i]);
    }

    printf("\n");

    hid_write(dev, usb_buf, 3);
}

void CorsairVoidProController::SetDirect(std::vector<RGBColor> colors)
{
    unsigned char usb_buf[CORSAIR_VOID_PRO_PACKET_SIZE] = {};

    memset(usb_buf, 0x00, CORSAIR_VOID_PRO_PACKET_SIZE);

    usb_buf[0]  = CORSAIR_VOID_PRO_REPORT_ID;    

    /*-----------------------------------------*\
    | Send 0x07 instead of 0x06 every seconds   |
    \*-----------------------------------------*/
    std::chrono::time_point<std::chrono::steady_clock> now = std::chrono::steady_clock::now();

    if(std::chrono::duration_cast<std::chrono::milliseconds>(now - last_direct_mode_packet_time).count() > 1000)
    {
        last_direct_mode_packet_time = now;
        usb_buf[1]  = DIRECT_MODE_VALUE_KEEPALIVE;
    }
    else
    {
        usb_buf[1]  = DIRECT_MODE_VALUE;
    }

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

    for(unsigned int i = 0; i < CORSAIR_VOID_PRO_PACKET_SIZE; i++)
    {
        printf("%02X ", usb_buf[i]);
    }

    printf("\n");

    hid_write(dev, usb_buf, CORSAIR_VOID_PRO_PACKET_SIZE);
}
