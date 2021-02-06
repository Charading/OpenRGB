/*-------------------------------------------------------------------*\
|  CMR6000Controller.cpp                                              |
|                                                                     |
|  Driver for Coolermaster based AMD Radeon GPU (6000 series)         |
|                                                                     |
|  Eric S (edbgon)          2nd Feb 2021                              |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "CMR6000Controller.h"

CMR6000Controller::CMR6000Controller(hid_device* dev_handle, char *_path)
{
    dev             = dev_handle;
    location        = _path;

    const int szTemp = 256;
    wchar_t tmpName[szTemp];

    hid_get_manufacturer_string(dev, tmpName, szTemp);
    std::wstring wName = std::wstring(tmpName);
    device_name        = std::string(wName.begin(), wName.end());

    hid_get_product_string(dev, tmpName, szTemp);
    wName  = std::wstring(tmpName);
    device_name.append(" ").append(std::string(wName.begin(), wName.end()));

    hid_get_serial_number_string(dev, tmpName, szTemp);
    wName  = std::wstring(tmpName);
    serial = std::string(wName.begin(), wName.end());

    GetStatus();        //When setting up device get current status
}

CMR6000Controller::~CMR6000Controller()
{
    if(dev)
    {
        hid_close(dev);
    }
}

void CMR6000Controller::GetStatus()
{
    unsigned char buffer[65]     = { 0x00 };
    int buffer_size              = (sizeof(buffer) / sizeof(buffer[0]));

    unsigned char cmdbuffer[65]  = { 0x00 };
    int cmdbuffer_size           = (sizeof(cmdbuffer) / sizeof(cmdbuffer[0]));

    // Request mode
    buffer[0x00] = 0x00;
    buffer[0x01] = 0x52;
    buffer[0x02] = 0xA0;
    buffer[0x03] = 0x01;
    buffer[0x04] = 0x00;
    buffer[0x05] = 0x00;
    buffer[0x06] = 0x03;

    hid_write(dev, buffer, buffer_size);
    hid_read(dev, buffer, buffer_size);

    cmdbuffer[0x00] = 0x00;
    cmdbuffer[0x01] = 0x52;
    cmdbuffer[0x02] = 0x2C;
    cmdbuffer[0x03] = 0x01;
    cmdbuffer[0x04] = 0x00;

    if(buffer[0x0A] == 0x00) // Static mode detected
    {
        cmdbuffer[0x05] = 0x00;

        hid_write(dev, cmdbuffer, cmdbuffer_size);
        hid_read(dev, cmdbuffer, cmdbuffer_size);

        current_mode  = CM_MR6000_MODE_STATIC;
        current_red   = cmdbuffer[0x0A];
        current_green = cmdbuffer[0x0B];
        current_blue  = cmdbuffer[0x0C];
    }
    else if(buffer[0x0A] == 0x01) // Breathing mode detected
    {
        cmdbuffer[0x05] = 0x01;

        hid_write(dev, cmdbuffer, cmdbuffer_size);
        hid_read(dev, cmdbuffer, cmdbuffer_size);

        current_mode  = CM_MR6000_MODE_BREATHE;
        current_red   = cmdbuffer[0x0A];
        current_green = cmdbuffer[0x0B];
        current_blue  = cmdbuffer[0x0C];
        current_speed = cmdbuffer[0x05];
    }
    else if(buffer[0x0A] == 0x02) // Color cycle mode detected
    {
        cmdbuffer[0x05] = 0x02;

        hid_write(dev, cmdbuffer, cmdbuffer_size);
        hid_read(dev, cmdbuffer, cmdbuffer_size);

        current_mode  = CM_MR6000_MODE_COLOR_CYCLE;
        current_speed = cmdbuffer[0x05];
    }
    else
    {
        //Code should never reach here however just in case there is a failure set something
        current_mode  = CM_MR6000_MODE_STATIC;
        current_red   = 0xFF;
        current_green = 0x00;
        current_blue  = 0x00;
        current_speed = MR6000_BREATHE_SPEED_NORMAL;
    }
}

std::string CMR6000Controller::GetDeviceName()
{
    return device_name;
}

std::string CMR6000Controller::GetSerial()
{
    return serial;
}

std::string CMR6000Controller::GetLocation()
{
    return("HID: " + location);
}

unsigned char CMR6000Controller::GetMode()
{
    return current_mode;
}

unsigned char CMR6000Controller::GetLedRed()
{
    return current_red;
}

unsigned char CMR6000Controller::GetLedGreen()
{
    return current_green;
}

unsigned char CMR6000Controller::GetLedBlue()
{
    return current_blue;
}

unsigned char CMR6000Controller::GetLedSpeed()
{
    return current_speed;
}

void CMR6000Controller::SetMode(unsigned char mode, unsigned char speed, unsigned char red, unsigned char green, unsigned char blue, unsigned char random)
{
    current_mode        = mode;
    current_speed       = speed;
    current_red         = red;
    current_green       = green;
    current_blue        = blue;
    current_random      = random;

    SendUpdate();
}

void CMR6000Controller::SendUpdate()
{    
    if(current_mode == CM_MR6000_MODE_OFF)
    {
        unsigned char buffer[65] = { 0x00 };
        int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

        buffer[0x01] = 0x41;
        buffer[0x02] = 0x43;
        
        hid_write(dev, buffer, buffer_size);
    }
    else
    {
        SendEnableCommand();

        unsigned char buffer[65] = { 0xFF };
        int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

        buffer[0x00] = 0x00;    //0x00 needs to be set because we've initialised 0xFF
        buffer[0x01] = 0x51;
        buffer[0x02] = 0x2C;
        buffer[0x03] = 0x01;
        buffer[0x04] = 0x00;    //0x00 needs to be set because we've initialised 0xFF
        buffer[0x05] = current_mode;
        buffer[0x06] = (current_mode == CM_MR6000_MODE_STATIC) ? 0xFF: current_speed;
        buffer[0x07] = (current_mode == CM_MR6000_MODE_BREATHE)? current_random : 0x00; //random (A0)
        buffer[0x08] = (current_mode == CM_MR6000_MODE_BREATHE)? 0x03 : 0xFF;
        //buffer[0x09] = 0xFF;
        //buffer[0x0A] = current_brightness; // FF = Max, 99 = Mid, 4C = Min
        buffer[0x0B] = (current_mode == CM_MR6000_MODE_COLOR_CYCLE) ? 0xFF : current_red;
        buffer[0x0C] = (current_mode == CM_MR6000_MODE_COLOR_CYCLE) ? 0xFF : current_green;
        buffer[0x0D] = (current_mode == CM_MR6000_MODE_COLOR_CYCLE) ? 0xFF : current_blue;
        buffer[0x0E] = 0x00;
        buffer[0x0F] = 0x00;

        hid_write(dev, buffer, buffer_size);

        SendColourConfig();
        SendApplyCommand();
    }
}

void CMR6000Controller::SendEnableCommand()
{
    unsigned char buffer[CM_6K_PACKET_SIZE] = { 0x00 };
    int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

    buffer[0x01] = 0x41;
    buffer[0x02] = 0x80;
    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_6K_INTERRUPT_TIMEOUT);
}

void CMR6000Controller::SendApplyCommand()
{
    unsigned char buffer[CM_6K_PACKET_SIZE] = { 0x00 };
    int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

    buffer[0x01] = 0x51;
    buffer[0x02] = 0x28;
    buffer[0x05] = 0xE0;
    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_6K_INTERRUPT_TIMEOUT);
}

void CMR6000Controller::SendColourConfig()
{
    unsigned char buffer[CM_6K_PACKET_SIZE] = { 0x00 };
    int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

    buffer[0x01] = 0x51;
    buffer[0x02] = 0xA0;
    buffer[0x03] = 0x01;
    buffer[0x06] = 0x03;
    buffer[0x09] = 0x05;
    buffer[0x0A] = 0x06;

    for(int i = 0x0B; i < 0x1A; i++)
    {
        buffer[i] = current_mode;
    }

    hid_write(dev, buffer, buffer_size);
    hid_read_timeout(dev, buffer, buffer_size, CM_6K_INTERRUPT_TIMEOUT);
}
