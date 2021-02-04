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
    hid_close(dev);
}

void CMR6000Controller::GetStatus()
{
    /*
    unsigned char buffer[0x41]  = { 0x00 };
    int buffer_size             = (sizeof(buffer) / sizeof(buffer[0]));
    buffer[1]                   = 0x07;

    hid_write(dev, buffer, buffer_size);
    hid_read(dev, buffer, buffer_size);

    if((buffer[0] == 0x80) && (buffer[1] == 0x05))
    {
        current_mode  = buffer[2];
        current_red   = buffer[3];
        current_green = buffer[4];
        current_blue  = buffer[5];

        for(int i = 0; (speed_mode_data[i] >= buffer[6] && i <= MP750_SPEED_FASTEST); i++)
        {
            current_speed = i;
        }
    }
    else
    {
        //Code should never reach here however just in case there is a failure set something
        current_mode  = CM_MP750_MODE_COLOR_CYCLE;      //Unicorn Spew
        current_red   = 0xFF;
        current_green = 0xFF;
        current_blue  = 0xFF;
        current_speed = MP750_SPEED_NORMAL;
    }
    */

    current_mode = CM_MR6000_MODE_STATIC;
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

void CMR6000Controller::SetMode(unsigned char mode, unsigned char speed)
{
    current_mode  = mode;

    SendUpdate();
}

void CMR6000Controller::SetColor(unsigned char red, unsigned char green, unsigned char blue)
{
    current_red   = red;
    current_green = green;
    current_blue  = blue;

    SendUpdate();
}

void CMR6000Controller::SendUpdate()
{
    
    unsigned char init_buf[64] = { 0x00 };
    int init_buffer_size = (sizeof(init_buf) / sizeof(init_buf[0]));
    
    if(current_mode == CM_MR6000_MODE_STATIC) {
        //Initialize LED (Official tool disables)
        
        init_buf[0x00] = 0x41;
        init_buf[0x01] = 0x01;
        
        hid_write(dev, init_buf, init_buffer_size);

        // Send data
        unsigned char buffer[64] = { 0xFF };
        for(int i = 0; i < 64; i++) {
            buffer[i] = 0xFF;
        }
        int buffer_size = (sizeof(buffer) / sizeof(buffer[0]));

        buffer[0x00] = 0x51;
        buffer[0x01] = 0x2C;
        buffer[0x02] = 0x01;
        buffer[0x03] = 0x00;
        buffer[0x04] = 0x00;
        buffer[0x06] = 0x00;

        buffer[0x0A] = current_red;
        buffer[0x0B] = current_green;
        buffer[0x0C] = current_blue;

        buffer[0x0D] = 0x00;
        buffer[0x0E] = 0x00;
        buffer[0x0F] = 0x00;

        hid_write(dev, buffer, buffer_size);
    }
    else if(current_mode == CM_MR6000_MODE_OFF) {
        
        init_buf[0x00] = 0x41;
        init_buf[0x01] = 0x43;
        
        hid_write(dev, init_buf, init_buffer_size);        
    }
    
}
