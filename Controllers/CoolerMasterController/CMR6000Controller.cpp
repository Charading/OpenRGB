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

void CMR6000Controller::SetMode(unsigned char mode, unsigned char speed, unsigned char red, unsigned char green, unsigned char blue)
{
    current_mode  = mode;
    current_speed = speed;
    current_red   = red;
    current_green = green;
    current_blue  = blue;

    SendUpdate();
}

void CMR6000Controller::SendUpdate()
{
    
    unsigned char buffer[65] = { 0x00 };
    int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

    if(current_mode == CM_MR6000_MODE_STATIC)
    {
        //Initialize
        buffer[0x01] = 0x41;
        buffer[0x02] = 0x80;
        hid_write(dev, buffer, buffer_size);

        buffer[0x01] = 0x51;
        buffer[0x02] = 0x28;
        buffer[0x05] = 0xE0;
        hid_write(dev, buffer, buffer_size);

        // Send data
        buffer[0x01] = 0x51;
        buffer[0x02] = 0x2C;
        buffer[0x03] = 0x01;
        buffer[0x04] = 0x00;
        buffer[0x05] = 0x00;
        buffer[0x06] = 0xFF;
        buffer[0x07] = 0x00;
        buffer[0x08] = 0xFF;
        buffer[0x09] = 0xFF;
        buffer[0x0A] = 0xFF;

        //buffer[0x0A] = current_brightness; // FF = Max, 99 = Mid, 4C = Min
        buffer[0x0B] = current_red;
        buffer[0x0C] = current_green;
        buffer[0x0D] = current_blue;

        for(int i = 0x0E; i < 0x41; i++)
        {
            buffer[i] = 0xFF;
        }

        hid_write(dev, buffer, buffer_size);

        for(int i = 0x00; i < 0x41; i++)
        {
            buffer[i] = 0x00;
        }

        //Apply changes
        buffer[0x01] = 0x51;
        buffer[0x02] = 0xA0;
        buffer[0x03] = 0x01;
        buffer[0x06] = 0x03;
        buffer[0x09] = 0x05;
        buffer[0x0A] = 0x06;
        hid_write(dev, buffer, buffer_size);

    }
    else if(current_mode == CM_MR6000_MODE_COLOR_CYCLE)
    {
        //Initialize
        buffer[0x01] = 0x41;
        buffer[0x02] = 0x80;
        hid_write(dev, buffer, buffer_size);
        
        // Send data
        buffer[0x01] = 0x51;
        buffer[0x02] = 0x2C;
        buffer[0x03] = 0x01;
        buffer[0x05] = 0x02;
        buffer[0x06] = current_speed;
        buffer[0x08] = 0xFF;
        buffer[0x09] = 0xFF;
        //buffer[0x0A] = current_brightness; // FF = Max, 99 = Mid, 4C = Min
        buffer[0x0A] = 0x7F;
        buffer[0x0B] = 0xFF;
        buffer[0x0C] = 0xFF;
        buffer[0x0D] = 0xFF;
        hid_write(dev, buffer, buffer_size);

        //Color config
        buffer[0x02] = 0xA0;
        buffer[0x05] = 0x00;
        buffer[0x06] = 0x03;
        buffer[0x08] = 0x00;
        buffer[0x09] = 0x05;
        buffer[0x0A] = 0x06;

        for(int i = 0x0B; i < 0x20; i++)
        {
            buffer[i] = 0x02;
        }
        hid_write(dev, buffer, buffer_size);
        
        for(int i = 0; i < buffer_size; i++)
        {
            buffer[i] = 0x00;
        }

        buffer[0x01] = 0x51;
        buffer[0x02] = 0x28;
        buffer[0x05] = 0xE0;
        hid_write(dev, buffer, buffer_size);

    }
    else if(current_mode == CM_MR6000_MODE_BREATHE)
    {
        //Initialize
        buffer[0x01] = 0x41;
        buffer[0x02] = 0x80;
        hid_write(dev, buffer, buffer_size);

        // Send data
        buffer[0x01] = 0x51;
        buffer[0x02] = 0x2C;
        buffer[0x03] = 0x01;
        buffer[0x04] = 0x00;
        buffer[0x05] = 0x01;
        buffer[0x06] = current_speed;
        buffer[0x07] = 0x20;
        buffer[0x08] = 0x03;
        buffer[0x09] = 0xFF;
        buffer[0x0A] = 0xFF;
        //buffer[0x0A] = current_brightness; // FF = Max, 99 = Mid, 4C = Min
        buffer[0x0B] = current_red;
        buffer[0x0C] = current_green;
        buffer[0x0D] = current_blue;

        for(int i = 0x11; i < 0x41; i++)
        {
            buffer[i] = 0xFF;
        }

        hid_write(dev, buffer, buffer_size);

        for(int i = 0x00; i < 0x41; i++)
        {
            buffer[i] = 0x00;
        }

        //Color config
        buffer[0x01] = 0x51;
        buffer[0x02] = 0xA0;
        buffer[0x03] = 0x01;
        buffer[0x06] = 0x03;
        buffer[0x09] = 0x05;
        buffer[0x0A] = 0x06;

        for(int i = 0x0B; i < 0x20; i++)
        {
            buffer[i] = 0x01;
        }
        hid_write(dev, buffer, buffer_size);
        
        for(int i = 0; i < buffer_size; i++)
        {
            buffer[i] = 0x00;
        }

        buffer[0x01] = 0x51;
        buffer[0x02] = 0x28;
        buffer[0x05] = 0xE0;
        hid_write(dev, buffer, buffer_size);

    }
    else if(current_mode == CM_MR6000_MODE_OFF)
    {
        
        buffer[0x00] = 0x00;
        buffer[0x01] = 0x41;
        buffer[0x02] = 0x43;
        
        hid_write(dev, buffer, buffer_size);
    }
    
}
