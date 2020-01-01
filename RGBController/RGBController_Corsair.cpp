/*-----------------------------------------*\
|  RGBController_Corsair.cpp                |
|                                           |
|  Generic RGB Interface for OpenAuraSDK    |
|  Corsair Vengeance RGB driver             |
|                                           |
|  Adam Honse (CalcProgrammer1) 6/13/2019   |
\*-----------------------------------------*/

#include "RGBController_Corsair.h"

int RGBController_Corsair::GetMode()
{
    return(CORSAIR_VENGEANCE_RGB_MODE_SINGLE);
}

void RGBController_Corsair::SetMode(int mode)
{
    corsair->SetMode(mode);
}

void RGBController_Corsair::SetCustomMode()
{

}

void RGBController_Corsair::SetAllLEDs(RGBColor color)
{
    unsigned char red = RGBGetRValue(color);
    unsigned char grn = RGBGetGValue(color);
    unsigned char blu = RGBGetBValue(color);

    corsair->SetAllColors(red, grn, blu);
}

void RGBController_Corsair::SetAllZoneLEDs(int zone, RGBColor color)
{
    unsigned char red = RGBGetRValue(color);
    unsigned char grn = RGBGetGValue(color);
    unsigned char blu = RGBGetBValue(color);

    for (std::size_t x = 0; x < zones[zone].map.size(); x++)
    {
        for (std::size_t y = 0; y < zones[zone].map[x].size(); y++)
        {
            corsair->SetLEDColor(zones[zone].map[x][y], red, grn, blu);
        }
    }
}

void RGBController_Corsair::SetLED(int led, RGBColor color)
{
    unsigned char red = RGBGetRValue(color);
    unsigned char grn = RGBGetGValue(color);
    unsigned char blu = RGBGetBValue(color);

    corsair->SetLEDColor(led, red, grn, blu);
}

void RGBController_Corsair::UpdateLEDs()
{

}

RGBController_Corsair::RGBController_Corsair(CorsairController* corsair_ptr)
{
    corsair = corsair_ptr;

    name = corsair->GetDeviceName();
    location = corsair->GetDeviceLocation();

    type = DEVICE_TYPE_DRAM;

    mode corsair_modes[CORSAIR_NUMBER_MODES];

    corsair_modes[0].name = "Static";
    corsair_modes[1].name = "Fade";
    corsair_modes[2].name = "Pulse";

    for (int i = 0; i < CORSAIR_NUMBER_MODES; i++)
    {
        modes.push_back(corsair_modes[i]);
    }

    for (unsigned int i = 0; i < corsair->GetLEDCount(); i++)
    {
        led* new_led = new led();

        new_led->name = "Corsair LED";

        leds.push_back(*new_led);
        colors.push_back(0x00000000);
    }

    zone new_zone;

    new_zone.name = "Corsair Zone";
    new_zone.type = ZONE_TYPE_SINGLE;

    std::vector<int> zone_row;

    for (unsigned int i = 0; i < corsair->GetLEDCount(); i++)
    {
        zone_row.push_back(i);
    }

    new_zone.map.push_back(zone_row);

    zones.push_back(new_zone);
}
