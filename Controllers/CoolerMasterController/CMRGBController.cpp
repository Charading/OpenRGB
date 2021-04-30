/*-------------------------------------------------------------------*\
|  CMRGBController.cpp                                          |
|                                                                     |
|  Driver for Coolermaster Small ARGB USB Controller                  |
|                                                                     |
|  Chris M (Dr_No)          31st Jan 2021                             |
|                                                                     |
|  Simple RGB device with 5 modes                                     |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_CMRGBController.h"
#include "CMRGBController.h"
#include <cstring>

CMRGBController::CMRGBController(hid_device* dev_handle, char *_path)
{
    const int szTemp = 256;
    wchar_t tmpName[szTemp];

    dev                     = dev_handle;
    location                = _path;

    hid_get_manufacturer_string(dev, tmpName, szTemp);
    std::wstring wName = std::wstring(tmpName);
    device_name = std::string(wName.begin(), wName.end());

    hid_get_product_string(dev, tmpName, szTemp);
    wName = std::wstring(tmpName);
    device_name.append(" ").append(std::string(wName.begin(), wName.end()));

    hid_get_serial_number_string(dev, tmpName, szTemp);
    wName = std::wstring(tmpName);
    serial = std::string(wName.begin(), wName.end());

    GetStatus();
}

void CMRGBController::GetStatus()
{
    // see masterplus-launch.pcapng to compare packet numbers
    unsigned char packets[7][CM_RGBC_PACKET_SIZE] =
    {
        { 0x41, 0x01 },                   // #285
        { 0x52, 0x28 },                   // #289
        { 0x41 },                         // #293
        { 0x52, 0x2C },                   // #297
        { 0x41, 0x80 },                   // #301
        { 0x52, 0xA8 },                   // #305
        { 0x52, 0x2C, 0x00, 0x00, 0x04 }, // #309
    };

    int buffer_size = (sizeof(packets[0]) / sizeof(packets[0][0]));

    for (int i = 5; i < CM_RGBC_PACKET_SIZE; i++)
    {
        packets[6][i] = 0xFF;
    }

    // at this point the packets should match what my untrained eyes see in masterplus-launch.pcapng

    for (int i = 0; i < 7; i++)
    {
//        hid_write(dev, packets[i], buffer_size);
//        hid_read_timeout(dev, packets[i], buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

        if (i == 1)
        {
            current_mode = packets[i][CM_RGBC_PACKET_OFFSET_MODE];
            packets[3][CM_RGBC_PACKET_OFFSET_MODE] = current_mode;
        }
        else if (i == 3) // the response packet is the same format as found in SetMode. See #297
        {
            current_mode  = packets[i][CM_RGBC_PACKET_OFFSET_MODE];
            current_speed = packets[i][CM_RGBC_PACKET_OFFSET_SPEED];
            current_brightness = packets[i][CM_RGBC_PACKET_OFFSET_BRIGHTNESS];
            current_red = packets[i][CM_RGBC_PACKET_OFFSET_COLOUR_1];
            current_green = packets[i][CM_RGBC_PACKET_OFFSET_COLOUR_1 + 1];
            current_blue = packets[i][CM_RGBC_PACKET_OFFSET_COLOUR_1 + 2];
        }
        else if (i == 5) // the response packet is the same format as the 4th packet in SetLedsDirect. See #307
        {
            // TODO: this is where the per-led colors are available for when it's in "Multiple" mode, but I'm not sure what to do with them
            // Note: It sends back this info even if it's not in "Multiple" mode atm
            RGBColor color1 = ToRGBColor(
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 1],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 2]);

            RGBColor color2 = ToRGBColor(
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 1],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 2]);

            RGBColor color3 = ToRGBColor(
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 1],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 2]);

            RGBColor color4 = ToRGBColor(
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 1],
                packets[i][CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 2]);
        }
    }

}

void CMRGBController::Send_Start_Stop(unsigned char byte_flag = 0x00)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { 0x00, 0x41 }; //Packets on Windows need a 0x00 if they don't use ReportIDs

    buffer[2] = byte_flag;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::Send_Save()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { 0x00, 0x50, 0x55 }; //Packets on Windows need a 0x00 if they don't use ReportIDs

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::SetMode(unsigned char mode, unsigned char speed, RGBColor colour)
{
    Send_Start_Stop(0x01);

    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { 0x00, 0x51, 0x2B }; //Packets on Windows need a 0x00 if they don't use ReportIDs

    // put the mode config info into the 2nd packet
    buffer[CM_RGBC_PACKET_OFFSET_MODE] = mode;
    buffer[CM_RGBC_PACKET_OFFSET_SPEED] = speed;
    buffer[CM_RGBC_PACKET_OFFSET_BRIGHTNESS] = (mode == CM_RGBC_MODE_OFF) ? 0x03 : CM_RGBC_BRIGHTNESS_MAX;
    buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1] = RGBGetRValue(colour);
    buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1 + 1] = RGBGetGValue(colour);
    buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1 + 2] = RGBGetBValue(colour);
    buffer[0x06] = (mode == CM_RGBC_MODE_BREATHING) ? 0x20 : 0x00;
    buffer[0x07] = (mode == CM_RGBC_MODE_STAR) ? 0x19: 0xFF;
    buffer[0x08] = 0xFF;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

    /*-----------------------------------------------------*\
    | Zero out buffer                                       |
    \*-----------------------------------------------------*/
    memset(buffer, 0x00, buffer_size);

    buffer[0x01] = 0x51;
    buffer[0x02] = 0x28;
    buffer[CM_RGBC_PACKET_OFFSET_MODE] = mode;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

    //Send_Save(); //Commented out for now
    Send_Start_Stop(0x00);
}

/*
 * This device uses different speed values for each mode. It's not clear if these values
 * are actually special or if any 0-255 value could be used and they just tuned their
 * numbers to ones that looked good for each individual mode.

unsigned char CMRGBController::GetSpeedValue(unsigned char mode, unsigned char speed)
{
    // TODO: bounds checks
    switch (mode)
    {
        case CM_RGBC_MODE_BREATHING:
            return CM_RGBC_SPEED_BREATHING[speed];
        case CM_RGBC_MODE_STAR:
            return CM_RGBC_SPEED_STAR[speed];
        case CM_RGBC_MODE_COLOR_CYCLE:
            return CM_RGBC_SPEED_COLOR_CYCLE[speed];
        default:
            return 0x05;
    }
}*/

void CMRGBController::SetLedsDirect(RGBColor *led_colours, unsigned int led_count)
{
    // TODO: led_count better be <= 4

    // see multiple.pcapng
    unsigned char colour_offsets[CM_RGBC_NUM_PORTS] = {
        CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1,
        CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2,
        CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3,
        CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4,
    };

    unsigned char packets[5][CM_RGBC_PACKET_SIZE] =
    {
        { 0x41, 0x80 },
        { 0x51, 0x28 },
        { 0x51, 0x30 },
        { 0x51, 0xA8 },
        { 0x51, 0x2C, 0x00, 0x00, 0x04, 0x00, 0x00, 0xFF,
          0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
    };

    for (int i = 16; i < CM_RGBC_PACKET_SIZE; i++)
    {
        packets[4][i] = 0xFF;
    }

    for (int i = 0; i < led_count; i++)
    {
        unsigned char cur_offset = colour_offsets[i];

        packets[3][cur_offset] = RGBGetRValue(led_colours[i]);
        packets[3][cur_offset + 1] = RGBGetGValue(led_colours[i]);
        packets[3][cur_offset + 2] = RGBGetBValue(led_colours[i]);
    }

}

std::string CMRGBController::GetDeviceName()
{
    return device_name;
}

std::string CMRGBController::GetSerial()
{
    return serial;
}

std::string CMRGBController::GetLocation()
{
    return("HID: " + location);
}

CMRGBController::~CMRGBController()
{
    if(dev)
    {
        hid_close(dev);
    }
}
