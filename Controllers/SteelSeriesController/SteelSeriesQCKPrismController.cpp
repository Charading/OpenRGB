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
    /* If a future device with the same protocol exists, some detection
     * improvements will need to be done here. */
    zone_count = 2;
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
    qckprism_brightness_pkt *header = (qckprism_brightness_pkt *)buf;
    memset(buf, 0x00, sizeof(buf));

    char brightness = abs(value*255);
    
    header->cmd = QCKPRISM_CMD_BRIGHTNESS;
    header->brightness = brightness;

    hid_write(dev, buf, sizeof(buf));
}


void SteelSeriesQCKPrismController::SetLightEffect
    (
    unsigned char   zone_id,
    unsigned char   effect
    )
{
    switch (effect)
    {
        case STEELSERIES_QCKPRISM_STATIC:
            break;
        case STEELSERIES_QCKPRISM_RAINBOW:
            break;
    }
}


void SteelSeriesQCKPrismController::SetLightEffectAll
    (
    unsigned char   effect
    )
{
    int i;
    for (i = 0; i < zone_count; i++) {
        SetLightEffect(i, effect);
    }

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

void SteelSeriesQCKPrismController::SetColorAll
    (
        unsigned char   red,
        unsigned char   green,
        unsigned char   blue
    )
{
    /* It is possible to batch these commands together instead of just doing a
     * loop */
    int i;

    for (i = 0; i < zone_count; i++) {
        SetColor(i, red, green, blue);
    }
}

