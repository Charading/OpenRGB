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

    dev         = dev_handle;
    location    = _path;

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

void CMRGBController::Send_Flow_Control(unsigned char byte_flag = 0x00)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { 0x00, CM_RGBC_OPCODE_OP_FLOW_CONTROL }; //Packets on Windows need a 0x00 if they don't use ReportIDs

    buffer[0x02] = byte_flag;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::Send_Apply()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { 0x00, CM_RGBC_OPCODE_OP_UNKNOWN_50, CM_RGBC_OPCODE_TYPE_UNKNOWN_55 }; //Packets on Windows need a 0x00 if they don't use ReportIDs

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::SendReadMode()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]     = CM_RGBC_OPCODE_OP_READ;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]   = CM_RGBC_OPCODE_TYPE_MODE;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

    current_mode = buffer[CM_RGBC_PACKET_OFFSET_MODE];
}

void CMRGBController::SendSetMode(unsigned char mode)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]     = CM_RGBC_OPCODE_OP_WRITE;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]   = CM_RGBC_OPCODE_TYPE_MODE;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MODE]   = mode;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::SendSetCustomColors(RGBColor color1, RGBColor color2, RGBColor color3, RGBColor color4)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    current_port1_color = color1;
    current_port2_color = color2;
    current_port3_color = color3;
    current_port4_color = color4;

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]                     = CM_RGBC_OPCODE_OP_WRITE;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]                   = CM_RGBC_OPCODE_TYPE_LED_INFO;

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1]      = RGBGetRValue(color1);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 1]  = RGBGetGValue(color1);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 2]  = RGBGetBValue(color1);

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2]      = RGBGetRValue(color2);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 1]  = RGBGetGValue(color2);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 2]  = RGBGetBValue(color2);

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3]      = RGBGetRValue(color3);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 1]  = RGBGetGValue(color3);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 2]  = RGBGetBValue(color3);

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4]      = RGBGetRValue(color4);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 1]  = RGBGetGValue(color4);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 2]  = RGBGetBValue(color4);

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::SendReadCustomColors()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]     = CM_RGBC_OPCODE_OP_READ;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]   = CM_RGBC_OPCODE_TYPE_LED_INFO;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

    current_port1_color = ToRGBColor(
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 1],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_1 + 2]);
    current_port2_color = ToRGBColor(
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 1],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_2 + 2]);
    current_port3_color = ToRGBColor(
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 1],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_3 + 2]);
    current_port4_color = ToRGBColor(
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 1],
        buffer[CM_RGBC_PACKET_OFFSET_MULTIPLE_COLOUR_4 + 2]);
}

void CMRGBController::SendSetConfig(unsigned char mode, unsigned char speed, unsigned char brightness, RGBColor colour1, RGBColor colour2, bool simplified=false, bool multilayer=false)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    current_mode = mode;
    current_speed = speed;
    current_brightness = brightness;
    current_mode_color = colour1;

    // handle special cases
    switch (mode) {
        case CM_RGBC_MODE_COLOR_CYCLE:
            brightness = 0xDF;
            colour1 = 0xFFFFFF;
            colour2 = 0x000000;
            break;
        case CM_RGBC_MODE_OFF:
            brightness = 0x03;
            break;
    }

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]             = CM_RGBC_OPCODE_OP_WRITE;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]           = simplified ? CM_RGBC_OPCODE_TYPE_CONFIG_SIMPLIFIED : CM_RGBC_OPCODE_TYPE_CONFIG_FULL;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MODE]           = mode;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_SPEED]          = speed;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_BRIGHTNESS]     = brightness;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_1]       = RGBGetRValue(colour1);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_1 + 1]   = RGBGetGValue(colour1);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_1 + 2]   = RGBGetBValue(colour1);

    // magic values, meaning unknown
    buffer[REPORT_ID_OFFSET + 0x06] = (mode == CM_RGBC_MODE_BREATHING) ? 0x20 : 0x00;
    buffer[REPORT_ID_OFFSET + 0x07] = (mode == CM_RGBC_MODE_STAR) ? 0x19: 0xFF;
    buffer[REPORT_ID_OFFSET + 0x08] = 0xFF;

    if (!simplified)
    {
        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MULTILAYER]     = multilayer ? 0x01 : 0x00;
        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2]       = RGBGetRValue(colour2);
        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2 + 1]   = RGBGetGValue(colour2);
        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2 + 2]   = RGBGetBValue(colour2);

        for (int i = REPORT_ID_OFFSET + 16; i < CM_RGBC_PACKET_SIZE; i++) {
            buffer[i] = 0xFF;
        }
    }

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

void CMRGBController::SendReadConfig(unsigned char mode)
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]             = CM_RGBC_OPCODE_OP_READ;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]           = CM_RGBC_OPCODE_TYPE_CONFIG_FULL;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MODE]           = mode;

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

    current_mode        = mode;
    current_speed       = buffer[CM_RGBC_PACKET_OFFSET_SPEED];
    current_brightness  = buffer[CM_RGBC_PACKET_OFFSET_BRIGHTNESS];
    current_mode_color  = ToRGBColor(
        buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1],
        buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1 + 1],
        buffer[CM_RGBC_PACKET_OFFSET_COLOUR_1 + 2]);
//    current_mode_color2 = ToRGBColor(
//        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2],
//        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2 + 1],
//        buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_COLOUR_2 + 2]);
}

void CMRGBController::SendCustomColorStart()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]     = CM_RGBC_OPCODE_OP_WRITE;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]   = 0x30;
    //buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MODE]   = 0x01; // not convinced this should be hardcoded, need some more test data

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
}

//void CMRGBController::SendEndPortConfig()
//{
//    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
//    unsigned char buffer[buffer_size]   = { };

//    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]         = CM_RGBC_OPCODE_OP_WRITE;
//    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]       = 0xA0;

//    // dunno what this is all about
//    buffer[REPORT_ID_OFFSET + 0x02]                             = 0x01;
//    buffer[REPORT_ID_OFFSET + 0x03]                             = 0x00;
//    buffer[REPORT_ID_OFFSET + 0x04]                             = 0x00;
//    buffer[REPORT_ID_OFFSET + 0x05]                             = 0x01;
//    buffer[REPORT_ID_OFFSET + 0x06]                             = 0x00;
//    buffer[REPORT_ID_OFFSET + 0x07]                             = 0x00;
//    buffer[REPORT_ID_OFFSET + 0x08]                             = 0x03;
//    buffer[REPORT_ID_OFFSET + 0x09]                             = 0x02;
//    buffer[REPORT_ID_OFFSET + 0x0A]                             = 0x01;

//    hid_write(dev, buffer, buffer_size);
//    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);
//}

//void CMRGBController::SendConfigPorts()
//{
//    SendCustomColorStart();
////    SendSetConfig(..., true) // port 1
////    SendSetConfig(..., true) // port 2
////    SendSetConfig(..., true) // port 3
////    SendSetConfig(..., true) // port 4
//    SendEndPortConfig();
//    SendSetMode(CM_RGBC_MODE_MULTILAYER);
//}

void CMRGBController::GetStatus()
{
    const unsigned char buffer_size     = CM_RGBC_PACKET_SIZE;
    unsigned char buffer[buffer_size]   = { };

    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_01);

    SendReadMode();

    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_00);

    SendReadConfig(current_mode);

    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_80);

    SendReadCustomColors();

    /*-----------------------------------------------------*\
    | Packet 7 (why?)                                       |
    \*-----------------------------------------------------*/
    memset(buffer, 0x00, buffer_size);
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_OP]     = CM_RGBC_OPCODE_OP_READ;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_TYPE]   = CM_RGBC_OPCODE_TYPE_CONFIG_FULL;
    buffer[REPORT_ID_OFFSET + CM_RGBC_PACKET_OFFSET_MODE]   = current_mode;
    for (int i = 6; i<CM_RGBC_PACKET_SIZE; i++) {
        buffer[i] = 0xFF;
    }

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_RGBC_INTERRUPT_TIMEOUT);

}

void CMRGBController::ReadModeConfig(unsigned char mode)
{
    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_00);
    SendReadConfig(mode);

    if (mode == CM_RGBC_MODE_MULTIPLE)
    {
        SendReadCustomColors();
    }
}

void CMRGBController::SetMode(unsigned char mode, unsigned char speed, unsigned char brightness, RGBColor colour)
{
    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_01);

    SendSetConfig(mode, speed, brightness, colour, 0x000000, true);

    SendSetMode(mode);

    Send_Apply();

    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_00);

    //GetStatus();
}

void CMRGBController::SetLedsDirect(RGBColor color1, RGBColor color2, RGBColor color3, RGBColor color4, bool saveChanges)
{
    Send_Flow_Control(CM_RGBC_OPCODE_FLOW_80);

    SendCustomColorStart();

    SendSetCustomColors(color1, color2, color3, color4);

    SendSetMode(CM_RGBC_MODE_MULTIPLE);

    SendSetConfig(CM_RGBC_MODE_MULTIPLE, 0x00, 0xFF, color1, 0x000000, false);

    if (saveChanges)
    {
        Send_Apply();
        Send_Flow_Control(CM_RGBC_OPCODE_FLOW_00);
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

unsigned char CMRGBController::GetMode() {
    return current_mode;
}

unsigned char CMRGBController::GetSpeed() {
    return current_speed;
}

unsigned char CMRGBController::GetBrightness() {
    return current_brightness;
}

RGBColor CMRGBController::GetModeColor() {
    return current_mode_color;
}

RGBColor CMRGBController::GetPort1Color() {
    return current_port1_color;
}

RGBColor CMRGBController::GetPort2Color() {
    return current_port2_color;
}

RGBColor CMRGBController::GetPort3Color() {
    return current_port3_color;
}

RGBColor CMRGBController::GetPort4Color() {
    return current_port4_color;
}



CMRGBController::~CMRGBController()
{
    if(dev)
    {
        hid_close(dev);
    }
}
