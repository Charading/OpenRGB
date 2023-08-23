/*---------------------------------------------------------*\
|  Roccat Kone Pure SEL                                     |
|                                                           |
|  Warren Dumortier (nwarrenfl@gmail.com), 23/03/2023       |
\*---------------------------------------------------------*/

#include "RoccatKonePureSelController.h"

RoccatKonePureSELController::RoccatKonePureSELController(hid_device* dev_handle, const hid_device_info& info)
{
    dev = dev_handle;
    location = info.path;
}

RoccatKonePureSELController::~RoccatKonePureSELController()
{
    hid_close(dev);
}

std::string RoccatKonePureSELController::GetDeviceName()
{
    return device_name;
}

std::string RoccatKonePureSELController::GetSerial()
{
    return serial;
}

std::string RoccatKonePureSELController::GetLocation()
{
    return("HID: " + location);
}

void RoccatKonePureSELController::SetMode(int mode, std::vector<RGBColor> colors)
{
    memset(usb_packet_buf, 0x00, ROCCAT_KONE_PURE_SEL_PACKET_SIZE);

    usb_packet_buf[0x00] = 0x00;
    usb_packet_buf[0x01] = 0x0C;
    usb_packet_buf[0x03] = 0x02;
    usb_packet_buf[0x08] = 0xF1;

    if(mode == ROCCAT_KONE_PURE_SEL_MODE_BREATHING)
    {
        usb_packet_buf[0x03] = 0x00;
        usb_packet_buf[0x08] = 0xF3;
    }
    else if(mode == ROCCAT_KONE_PURE_SEL_MODE_STATIC)
    {
        int count = 0xF1;

        // red
        if(RGBGetRValue(colors[0]) == 0xFF)
        {
            usb_packet_buf[0x04] = 0xFF;
            count++;
        }

        // green
        if(RGBGetGValue(colors[0]) == 0xFF)
        {
            usb_packet_buf[0x05] = 0xFF;
            count++;
        }

        // blue
        if(RGBGetBValue(colors[0]) == 0xFF)
        {
            usb_packet_buf[0x06] = 0xFF;
            count++;
        }

        usb_packet_buf[0x08] = count;
    }
}

void RoccatKonePureSELController::SendUpdate()
{
    hid_send_feature_report(dev, usb_packet_buf, ROCCAT_KONE_PURE_SEL_PACKET_SIZE);
}
