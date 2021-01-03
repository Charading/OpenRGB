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
        int zone_id,
        unsigned char   red,
        unsigned char   green,
        unsigned char   blue
    )
{
    unsigned char buf[524];
    qckprism_color_pkt *header = (qckprism_color_pkt *)buf;
    qckprism_color_body *m1;
    m1 = (qckprism_color_body *)&buf[sizeof(qckprism_color_pkt)];
 
    memset(buf, 0x00, sizeof(buf));

    header->cmd = QCKPRISM_CMD_COLOR;
    header->count = 1;

    m1->r = red;
    m1->g = green;
    m1->b = blue;
    m1->x = 0xff;
    m1->unknown1 = 0x32c8;
    m1->cycle = 1;
    m1->zone = zone_id;

    hid_send_feature_report(dev, buf, sizeof(buf));
    Brightness(1.0);
    Apply();
}

/* Currently not used, but the LED commands can be batched together. */
void SteelSeriesQCKPrismController::SetColorAll
    (
        unsigned char   red,
        unsigned char   green,
        unsigned char   blue
    )
{
    unsigned char buf[524];
    qckprism_color_pkt *header = (qckprism_color_pkt *)buf;
    qckprism_color_body *m1, *m2;
    m1 = (qckprism_color_body *)&buf[sizeof(qckprism_color_pkt)];
    m2 = (qckprism_color_body *)&buf[sizeof(qckprism_color_pkt) + sizeof(qckprism_color_body)];
    
    memset(buf, 0x00, sizeof(buf));

    header->cmd = QCKPRISM_CMD_COLOR;
    header->count = 2;

    // First LED
    m1->r = red;
    m1->g = green;
    m1->b = blue;
    m1->x = 0xff;
    m1->unknown1 = 0x32c8;
    m1->cycle = 1;
    m1->zone = 0; // Zone ID

    // Second LED
    m2->r = red;
    m2->g = green;
    m2->b = blue;
    m2->x = 0xff;
    m2->unknown1 = 0x32c8;
    m2->cycle = 1;
    m2->zone = 1; // Zone ID

    hid_send_feature_report(dev, buf, sizeof(buf));
    Brightness(1.0);
    Apply();
}

