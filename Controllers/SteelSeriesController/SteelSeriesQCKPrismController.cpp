/*-----------------------------------------*\
|  SteelSeriesQCKPrismController.h          |
|                                           |
|  Definitions and types for SteelSeries    |
|  QCK Prism lighting controller            |
|                                           |
|  B Horn (bahorn) 30/12/2020               |
\*-----------------------------------------*/

#include "SteelSeriesQCKPrismController.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

SteelSeriesQCKPrismController::SteelSeriesQCKPrismController
    (
    hid_device*         dev_handle,
    const char*         path
    )
{
    dev         = dev_handle;
    location    = path;
}

SteelSeriesQCKPrismController::~SteelSeriesQCKPrismController()
{

}

std::string SteelSeriesQCKPrismController::GetDeviceLocation()
{
    return("HID: " + location);
}

char* SteelSeriesQCKPrismController::GetDeviceName()
{
    return device_name;
}

std::string SteelSeriesQCKPrismController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

/* Saves to the internal configuration */
void SteelSeriesQCKPrismController::Save()
{

}

/* Applies the effect to the live device. */
void SteelSeriesQCKPrismController::Apply()
{
        
    unsigned char buf[64];
    memset(buf, 0x00, sizeof(buf));

    buf[0x00] = 0x0d;

    hid_write(dev, buf, sizeof(buf));
}

void SteelSeriesQCKPrismController::Brightness(float value)
{
    
    unsigned char buf[64];
    memset(buf, 0x00, sizeof(buf));

    char brightness = abs(value*255);
    
    buf[0x00] = 0x0c;
    buf[0x02] = brightness;

    hid_write(dev, buf, sizeof(buf));
}


void SteelSeriesQCKPrismController::SetLightEffect
    (
    unsigned char   zone_id,
    unsigned char   effect
    )
{

}


void SteelSeriesQCKPrismController::SetLightEffectAll
    (
    unsigned char   effect
    )
{

}

void SteelSeriesQCKPrismController::SetColor
    (
        unsigned char   red1,
        unsigned char   green1,
        unsigned char   blue1,
        unsigned char   red2,
        unsigned char   green2,
        unsigned char   blue2
    )
{
    unsigned char buf[524];
    
    memset(buf, 0x00, sizeof(buf));

    buf[0x00] = 0x0e;
    buf[0x01] = 0x00;
    buf[0x02] = 0x02; // LED/Zone count?
    buf[0x03] = 0x00;
    // First LED
    buf[0x04] = red1;
    buf[0x05] = green1;
    buf[0x06] = blue1;
    buf[0x07] = 0xff; // Thinking brightness?
    buf[0x08] = 0x32;
    buf[0x09] = 0xc8; // thinking these are flags or something?
    buf[0x0a] = 0x01;
    buf[0x0b] = 0x00;
    buf[0x0c] = 0x00;
    buf[0x0d] = 0x01;
    buf[0x0e] = 0x00;
    buf[0x0f] = 0x00; // Zone ID

    // Second LED
    buf[0x10] = red2;
    buf[0x11] = green2;
    buf[0x12] = blue2;
    buf[0x13] = 0xff;
    buf[0x14] = 0x32;
    buf[0x15] = 0xc8; // thinking these are flags or something?
    buf[0x16] = 0x00;
    buf[0x17] = 0x00;
    buf[0x18] = 0x00;
    buf[0x19] = 0x01;
    buf[0x1a] = 0x00;
    buf[0x1b] = 0x01; // Zone ID


    hid_send_feature_report(dev, buf, sizeof(buf));
    Brightness(1.0);
    Apply();
}

