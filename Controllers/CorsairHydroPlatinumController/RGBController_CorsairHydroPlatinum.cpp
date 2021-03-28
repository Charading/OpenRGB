/*-------------------------------------------------------------------*\
|  RGBController_CorsairHydroPlatinum.cpp                             |
|                                                                     |
|  Driver for Corsair Hydro Platinum AIO Coolers                      |
|                                                                     |
|  Kasper         28th March 2021                                     |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_CorsairHydroPlatinum.h"

RGBController_CorsairHydroPlatinum::RGBController_CorsairHydroPlatinum(CorsairHydroPlatinumController* corsair_ptr)
{
    corsair = corsair_ptr;

    vendor      = "Corsair";
    description = "Corsair Hydro Platinum Series Device";
    type        = DEVICE_TYPE_COOLER;
    location = corsair->GetLocation();
    version = corsair->GetFirmwareString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

void RGBController_CorsairHydroPlatinum::SetupZones()
{
    /*-------------------------------------------------*\
    | Only set LED count on the first run               |
    \*-------------------------------------------------*/
    bool first_run = false;

    if(zones.size() == 0)
    {
        first_run = true;
    }

    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    colors.clear();
    zones.resize(2);

    zones[0].name     = "CPU Block";
    zones[0].type     = ZONE_TYPE_LINEAR;
    zones[0].leds_min   = 0;
    zones[0].leds_max   = 16;

    zones[1].name     = "Fans";
    zones[1].type     = ZONE_TYPE_LINEAR;
    zones[1].leds_min   = 0;
    zones[1].leds_max   = 32;

    for (unsigned int zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        if(first_run)
        {
            zones[zone_idx].leds_count = 0;
        }

        zones[zone_idx].matrix_map = NULL;

        for (unsigned int led_idx = 0; led_idx < zones[zone_idx].leds_count; led_idx++)
        {
            led new_led;
            new_led.name = zones[zone_idx].name + " " + std::to_string(led_idx);;
            leds.push_back(new_led);
        }
    }

    SetupColors();
}

void RGBController_CorsairHydroPlatinum::ResizeZone(int zone, int new_size)
{
    if((size_t) zone >= zones.size())
    {
        return;
    }

    if(((unsigned int)new_size >= zones[zone].leds_min) && ((unsigned int)new_size <= zones[zone].leds_max))
    {
        zones[zone].leds_count = new_size;

        SetupZones();
    }
}

void RGBController_CorsairHydroPlatinum::DeviceUpdateLEDs()
{
    corsair->SetupColors(colors);
}

void RGBController_CorsairHydroPlatinum::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairHydroPlatinum::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairHydroPlatinum::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_CorsairHydroPlatinum::DeviceUpdateMode()
{

}
