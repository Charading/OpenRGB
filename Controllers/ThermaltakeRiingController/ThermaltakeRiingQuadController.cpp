/*-------------------------------------------------------------------*\
|  ThermaltakeRiingQuadController.cpp                                 |
|                                                                     |
|  Driver for Thermaltake Riing Quad Controller                       |
|                                                                     |
|  Chris M (Dr_No)          15th Feb 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "ThermaltakeRiingQuadController.h"
#include <cstring>

ThermaltakeRiingQuadController::ThermaltakeRiingQuadController(hid_device* dev_handle, const char* path)
{
    wchar_t tmpName[HID_MAX_STR];

    dev         = dev_handle;
    location    = path;

    hid_get_manufacturer_string(dev, tmpName, HID_MAX_STR);
    std::wstring wName = std::wstring(tmpName);
    device_name = std::string(wName.begin(), wName.end());

    hid_get_product_string(dev, tmpName, HID_MAX_STR);
    wName = std::wstring(tmpName);
    device_name.append(" ").append(std::string(wName.begin(), wName.end()));

    hid_get_serial_number_string(dev, tmpName, HID_MAX_STR);
    wName = std::wstring(tmpName);
    serial = std::string(wName.begin(), wName.end());

    SendInit();
}

ThermaltakeRiingQuadController::~ThermaltakeRiingQuadController()
{
    hid_close(dev);
}

std::string ThermaltakeRiingQuadController::GetDeviceName()
{
    return device_name;
}

std::string ThermaltakeRiingQuadController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string ThermaltakeRiingQuadController::GetSerial()
{
    return(serial);
}

void ThermaltakeRiingQuadController::SetChannelLEDs(uint8_t zone, RGBColor * colors, unsigned int num_colors)
{
    unsigned char* color_data = new unsigned char[3 * num_colors];

    for(std::size_t color = 0; color < num_colors; color++)
    {
        int color_idx = color * 3;
        color_data[color_idx + 0] = RGBGetGValue(colors[color]);    //Green First
        color_data[color_idx + 1] = RGBGetRValue(colors[color]);
        color_data[color_idx + 2] = RGBGetBValue(colors[color]);
    }

    SendRGB(zone, num_colors, color_data);

    delete[] color_data;
}

void ThermaltakeRiingQuadController::SetMode(unsigned char mode, unsigned char speed)
{
    current_mode        = mode;
    current_speed       = speed;
}

void ThermaltakeRiingQuadController::SendInit()
{
    unsigned char buffer[TT_QUAD_PACKET_SIZE]   = { 0x00, 0xFE, 0x33};
    int buffer_size                             = (sizeof(buffer) / sizeof(buffer[0]));

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, TT_QUAD_INTERRUPT_TIMEOUT);
}

void ThermaltakeRiingQuadController::SendRGB(unsigned char zone, unsigned char num_colors, unsigned char* color_data)
{
    unsigned char buffer[TT_QUAD_PACKET_SIZE]   = { 0x00, 0x32, 0x52};  // 0x33, 0x51, Zone ??
    int buffer_size                             = (sizeof(buffer) / sizeof(buffer[0]));

    buffer[TT_QUAD_ZONE_BYTE]                   = zone;
    buffer[TT_QUAD_MODE_BYTE]                   = current_mode + ( current_speed & 0x03 );
    memcpy(&buffer[TT_QUAD_DATA_BYTE], color_data, (num_colors * 3));   //Copy in GRB color data

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, TT_QUAD_INTERRUPT_TIMEOUT);
}
