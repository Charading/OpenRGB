/*-----------------------------------------*\
|  SteelSeriesSenseiTenController.h         |
|                                           |
|  Definitions and types for SteelSeries    |
|  Sensei TEN lighting controller           |
|  Based on Rival controller by             |
|  B Horn (bahorn) 13/5/2020                |
\*-----------------------------------------*/

#include "SteelSeriesSenseiTenController.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>


static void send_usb_msg(hid_device* dev, char * data_pkt, unsigned int size)
{
    char* usb_pkt = new char[size + 1];
    
    usb_pkt[0] = 0x00;
    for(unsigned int i = 1; i < size + 1; i++)
    {
        usb_pkt[i] = data_pkt[i-1];
    }
    
    hid_write(dev, (unsigned char *)usb_pkt, size + 1);
    
    delete usb_pkt;
}

SteelSeriesSenseiTenController::SteelSeriesSenseiTenController
    (
    hid_device*         dev_handle,
    steelseries_type    proto_type,
    const char*         path
    )
{
    dev         = dev_handle;
    location    = path;
    proto       = proto_type;
}

SteelSeriesSenseiTenController::~SteelSeriesSenseiTenController()
{
    hid_close(dev);
}

std::string SteelSeriesSenseiTenController::GetDeviceLocation()
{
    return("HID: " + location);
}

char* SteelSeriesSenseiTenController::GetDeviceName()
{
    return device_name;
}

std::string SteelSeriesSenseiTenController::GetSerialString()
{
    wchar_t serial_string[128];
    hid_get_serial_number_string(dev, serial_string, 128);

    std::wstring return_wstring = serial_string;
    std::string return_string(return_wstring.begin(), return_wstring.end());

    return(return_string);
}

steelseries_type SteelSeriesSenseiTenController::GetMouseType()
{
    return proto;
}

/* Saves to the internal configuration */
void SteelSeriesSenseiTenController::Save()
{
    char usb_buf[9];
    memset(usb_buf, 0x00, sizeof(usb_buf));
    usb_buf[0x00]       = 0x09;
    send_usb_msg(dev, usb_buf, 9);
}


void SteelSeriesSenseiTenController::SetLightEffect
    (
    unsigned char   zone_id,
    unsigned char   effect,
    unsigned char   speed,
    unsigned char   red,
    unsigned char   green,
    unsigned char   blue
    )
{
    char usb_buf[65];
    memset(usb_buf, 0x00, sizeof(usb_buf));
    char dur1;
    char dur2;

    switch(effect){

    case STEELSERIES_SENSEI_BREATHING:
        switch(speed){
        case STEELSERIES_SENSEI_EFFECT_BREATHING_MIN:
            dur1 = 0x27;
            dur2 = 0x10; //10 sec cycle
            break;

        case STEELSERIES_SENSEI_EFFECT_BREATHING_MID:
            dur1 = 0x13;
            dur2 = 0x88; //5 sec cycle
            break;

        case STEELSERIES_SENSEI_EFFECT_BREATHING_MAX:
            dur1 = 0x09;
            dur2 = 0xc4; //2.5 sec cycle
            break;
        }
        usb_buf[0x00]       = 0x5B; //command byte
        usb_buf[0x02]       = zone_id;
        usb_buf[0x04]       = dur1; //duration in ms 1st byte
        usb_buf[0x03]       = dur2; //duration in ms 2nd byte
        usb_buf[0x1B]       = 0x03; //Number of colors

        /* Original software duplicates these RGB bytes, but seems unnecessary
        usb_buf[0x1C]       = red;
        usb_buf[0x1D]       = green;
        usb_buf[0x1E]       = blue;
        */

        usb_buf[0x1F]       = red;
        usb_buf[0x20]       = green;
        usb_buf[0x21]       = blue;
        usb_buf[0x26]       = 0x7F; //percent of duration out of 0xFF
        usb_buf[0x27]       = red;
        usb_buf[0x28]       = green;
        usb_buf[0x29]       = blue;
        usb_buf[0x2A]       = 0x7F; //percent of duration out of 0xFF
        break;

    case STEELSERIES_SENSEI_RAINBOW:
        switch(speed){
        case STEELSERIES_SENSEI_EFFECT_RAINBOW_MIN:
            dur1 = 0x4E;
            dur2 = 0x20; //20 sec cycle
            break;

        case STEELSERIES_SENSEI_EFFECT_RAINBOW_MID:
            dur1 = 0x27;
            dur2 = 0x10; //10 sec cycle
            break;

        case STEELSERIES_SENSEI_EFFECT_RAINBOW_MAX:
            dur1 = 0x13;
            dur2 = 0x88; //5 sec cycle
            break;
        }
        usb_buf[0x00]       = 0x5B; //command byte
        usb_buf[0x02]       = zone_id;
        usb_buf[0x04]       = dur1; //duration in ms 1st byte
        usb_buf[0x03]       = dur2; //duration in ms 2nd byte
        usb_buf[0x1B]       = 0x07; //Number of colors

        /* Original software duplicates these RGB bytes, but seems unnecessary
        usb_buf[0x1C]       = red;
        usb_buf[0x1D]       = green;
        usb_buf[0x1E]       = blue;
        */
        usb_buf[0x1C]       = 0xFF;
        usb_buf[0x1F]       = 0xFF;
        usb_buf[0x22]       = 0x14;
        usb_buf[0x23]       = 0xFF;
        usb_buf[0x24]       = 0xFF;
        usb_buf[0x26]       = 0x2B; //percent of duration out of 0xFF
        usb_buf[0x28]       = 0xFF;
        usb_buf[0x2A]       = 0x2B;
        usb_buf[0x2C]       = 0xFF;
        usb_buf[0x2D]       = 0xFF; //percent of duration out of 0xFF
        usb_buf[0x2E]       = 0x28;
        usb_buf[0x31]       = 0xFF;
        usb_buf[0x32]       = 0x2B;
        usb_buf[0x33]       = 0xFF;
        usb_buf[0x35]       = 0xFF;
        usb_buf[0x36]       = 0x2B;
        usb_buf[0x37]       = 0xFF;
        usb_buf[0x3A]       = 0x14;
        break;
    }

    send_usb_msg(dev, usb_buf, sizeof(usb_buf));
}


void SteelSeriesSenseiTenController::SetLightEffectAll
    (
    unsigned char   effect,
    unsigned char   speed,
    unsigned char   red,
    unsigned char   green,
    unsigned char   blue
    )
{

    SetLightEffect(0, effect, speed, red, green, blue);
    SetLightEffect(1, effect, speed, red, green, blue);

}


void SteelSeriesSenseiTenController::SetColor
    (
    unsigned char   zone_id,
    unsigned char   red,
    unsigned char   green,
    unsigned char   blue
    )
{
    char usb_buf[65];
    memset(usb_buf, 0x00, sizeof(usb_buf));
    usb_buf[0x00]       = 0x5B; //command byte
    usb_buf[0x02]       = zone_id;

    /* Original software duplicates these RGB bytes, but seems unnecessary
    usb_buf[0x1C]       = red;
    usb_buf[0x1D]       = green;
    usb_buf[0x1E]       = blue;
    */

    usb_buf[0x1F]       = red;
    usb_buf[0x20]       = green;
    usb_buf[0x21]       = blue;
    usb_buf[0x13]       = 0x01; //Static color flag
    usb_buf[0x1B]       = 0x01; //Number of colors
    send_usb_msg(dev, usb_buf, sizeof(usb_buf));
}

void SteelSeriesSenseiTenController::SetColorAll
    (
        unsigned char   red,
        unsigned char   green,
        unsigned char   blue
    )
{

    switch(proto)
    {
        case SENSEI_TEN:
            SetColor(0, red, green, blue);
            SetColor(1, red, green, blue);
            break;

        default:
            break;
    }
}

