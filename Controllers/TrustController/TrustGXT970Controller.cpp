/*-----------------------------------------*\
|  TrustGXT970Controller.cpp                |
|                                           |
|  Driver for Trust GXT 970 controller      |
|                                           |
|  David Heidelberg 12/31/2022              |
\*-----------------------------------------*/
#include "TrustGXT970Controller.h"
#include <string.h>

TrustGXT970Controller::TrustGXT970Controller(hid_device* dev_handle, const hid_device_info& info)
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

}

TrustGXT970Controller::~TrustGXT970Controller()
{
    hid_close(dev);
}

std::string TrustGXT970Controller::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string TrustGXT970Controller::GetSerialString()
{
    return(serial_number);
}

std::string TrustGXT970Controller::GetFirmwareVersion()
{
    return(version);
}

void TrustGXT970Controller::SetMode(RGBColor color, unsigned char brightness, unsigned char speed, unsigned char mode_value)
{
    /*-----------------------------------------*\
    | Create and zero out the buffer            |
    \*-----------------------------------------*/
    unsigned char usb_buf[TRUST_GXT_970_REPORT_SIZE];
    memset(usb_buf, 0x00, sizeof(usb_buf));

    usb_buf[0]          = TRUST_GXT_970_REPORT_ID;
    usb_buf[1]          = 0x11;
    usb_buf[3]          = 0x7B;
    usb_buf[8]          = 0x64;
    usb_buf[9]          = 0x0B;
    usb_buf[10]         = 0x03;
    usb_buf[11]         = 0x46;
    usb_buf[12]         = 0xc0;
    // DPI stuff TRUST_GXT_970_DPI_MIN and MAX
    usb_buf[13]         = 0x07; // DPI 1
    usb_buf[14]         = 0x0f; // DPI 2
    usb_buf[15]         = 0x17; // DPI 3
    usb_buf[16]         = 0x1f; // DPI 4
    usb_buf[17]         = 0x27; // DPI 5
    usb_buf[18]         = 0x31; // DPI 6
    // DPI color profile 1
    usb_buf[29]         = RGBGetRValue(color);
    usb_buf[30]         = RGBGetGValue(color);
    usb_buf[31]         = RGBGetBValue(color);
    // DPI color profile 2
    usb_buf[32]         = RGBGetRValue(color);
    usb_buf[33]         = RGBGetGValue(color);
    usb_buf[34]         = RGBGetBValue(color);
    // DPI color profile 3
    usb_buf[35]         = RGBGetRValue(color);
    usb_buf[36]         = RGBGetGValue(color);
    usb_buf[37]         = RGBGetBValue(color);
    // DPI color profile 4
    usb_buf[38]         = RGBGetRValue(color);
    usb_buf[39]         = RGBGetGValue(color);
    usb_buf[40]         = RGBGetBValue(color);
    // DPI color profile 5
    usb_buf[41]         = RGBGetRValue(color);
    usb_buf[42]         = RGBGetGValue(color);
    usb_buf[43]         = RGBGetBValue(color);
    // DPI color profile 6
    usb_buf[44]         = RGBGetRValue(color);
    usb_buf[45]         = RGBGetGValue(color);
    usb_buf[46]         = RGBGetBValue(color);

    usb_buf[53]         = mode_value;
    usb_buf[54]         = 0x32;
    usb_buf[56]         = brightness;

    usb_buf[57]      = RGBGetRValue(color);
    usb_buf[58]      = RGBGetGValue(color);
    usb_buf[59]      = RGBGetBValue(color);

    usb_buf[60]         = 0x41; // related to breating
    usb_buf[61]         = 0x06; // related to breating

    // FIXME breating colorset 1
    usb_buf[62]      = RGBGetRValue(color);
    usb_buf[63]      = RGBGetGValue(color);
    usb_buf[64]      = RGBGetBValue(color);

    // FIXME breating colorset 2
    usb_buf[65]      = RGBGetRValue(color);
    usb_buf[66]      = RGBGetGValue(color);
    usb_buf[67]      = RGBGetBValue(color);

    // FIXME breating colorset 3
    usb_buf[68]      = RGBGetRValue(color);
    usb_buf[69]      = RGBGetGValue(color);
    usb_buf[70]      = RGBGetBValue(color);

    // FIXME breating colorset 4
    usb_buf[71]      = RGBGetRValue(color);
    usb_buf[72]      = RGBGetGValue(color);
    usb_buf[73]      = RGBGetBValue(color);

    // FIXME breating colorset 5
    usb_buf[74]      = RGBGetRValue(color);
    usb_buf[75]      = RGBGetGValue(color);
    usb_buf[76]      = RGBGetBValue(color);

    // FIXME breating colorset 6
    usb_buf[77]      = RGBGetRValue(color);
    usb_buf[78]      = RGBGetGValue(color);
    usb_buf[79]      = RGBGetBValue(color);

    // FIXME breating colorset 7
    usb_buf[80]      = 0xff;
    usb_buf[81]      = 0xff;
    usb_buf[82]      = 0xff;


    /*-----------------------------------------*\
    | Send the feature report                   |
    \*-----------------------------------------*/
    hid_send_feature_report(dev, usb_buf, TRUST_GXT_970_REPORT_SIZE);
}
