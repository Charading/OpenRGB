#include "HavitKB395LController.h"
#include "LogManager.h"

#include <cstring>

HavitKB395LController::HavitKB395LController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;

}

HavitKB395LController::~HavitKB395LController()
{
    hid_close(dev);
}

std::string HavitKB395LController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string HavitKB395LController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

void HavitKB395LController::SendStaticColor(unsigned char red, unsigned char green, unsigned char blue, unsigned char brightness)
{
    /*--------------------------------------------------------*\
    | Retrieve info for constructing Static Mode Packet        |
    \*--------------------------------------------------------*/
    unsigned char base_color = base_color_map[red][green][blue];
    unsigned char packet_color = color_index_map[red][green][blue];

    /*--------------------------------------------------------*\
    | Constructing Static Mode Packet                          |
    \*--------------------------------------------------------*/
    unsigned char usb_buf[9] = {0x00, 0x08, 0x02, 0x01, 0x0a, brightness, packet_color, 0x02, base_color - brightness };
    
    /*--------------------------------------------------------*\
    | Send Static Mode Packet                                  |
    \*--------------------------------------------------------*/
    auto mysize = sizeof(usb_buf) / sizeof(usb_buf[0]);
    hid_send_feature_report(dev, usb_buf, mysize);

}
