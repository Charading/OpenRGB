/*-----------------------------------------*\
|  RGBController_CorsairCapellix.cpp        |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Capellix Series                          |
|                                           |
|  Jeff P.                                  |
\*-----------------------------------------*/

#include "RGBController_CorsairCommanderCore.h"
#include <iostream>

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

RGBController_CorsairCommanderCore::RGBController_CorsairCommanderCore(CorsairCommanderCoreController* corsair_ptr)
{
    corsair = corsair_ptr;

    vendor      = "Corsair";
    description = "Corsair Capellix Series Device";
    type        = DEVICE_TYPE_COOLER;
    //location    = corsair->GetLocation();

    SetupZones();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);
}

RGBController_CorsairCommanderCore::~RGBController_CorsairCommanderCore()
{
    delete corsair;
}

void RGBController_CorsairCommanderCore::SetupZones()
{
    std::atomic<bool> first_run;
    first_run = 0;
    fanleds = corsair->DetectRGBFans();
    std::cout<<"Fanleds size: "<<fanleds.size()<<std::endl;
    if(zones.size() == 0)
    {
        first_run = 1;
    }
    std::cout<<"Begin zone setup"<<std::endl;
    int TotalFans=0;
    std::cout<<"Adding pump zone"<<std::endl;
    zones.resize(fanleds.size());

    for(int i = 0; i<fanleds.size(); i++){
        switch(fanleds[i]){
        case 0:
            std::cout<<"No RGB in port "<<i<<std::endl;
            break;
        case 29:
            std::cout<<"Adding pump"<<std::endl;
            zones[i].name               = "Pump";
            zones[i].type               = ZONE_TYPE_LINEAR;
            zones[i].leds_min           = 0;
            zones[i].leds_max           = 29;
            //zones[i].leds_count         = 29;
            break;
        default:
            std::cout<<"Adding fan in port "<<i<<std::endl;
            zones[i].name               = "Fan " + std::to_string(TotalFans+1);
            zones[i].type               = ZONE_TYPE_LINEAR;
            zones[i].leds_min           = 0;
            zones[i].leds_max           = 34;
            //zones[i].leds_count         = 34;
            TotalFans++;
            break;
        }
        if(first_run)
        {
            zones[i].leds_count = 0;
        }
    }
    std::cout<<"Finish zone setup"<<std::endl;

    leds.clear();
    colors.clear();

    for (unsigned int zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        std::cout<<"Adding led zone "<<zone_idx<<std::endl;
        for (unsigned int led_idx = 0; led_idx < zones[zone_idx].leds_count; led_idx++)
        {
            led new_led;
            new_led.name = zones[zone_idx].name + " LED " + std::to_string(led_idx+1);
            leds.push_back(new_led);
        }
    }
    std::cout<<"Zone setup done"<<std::endl;
    SetupColors();
}

void RGBController_CorsairCommanderCore::ResizeZone(int zone, int new_size)
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

void RGBController_CorsairCommanderCore::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_CorsairCommanderCore::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairCommanderCore::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairCommanderCore::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_CorsairCommanderCore::DeviceUpdateMode()
{
        switch(modes[active_mode].value)
        {
        case CORSAIR_COMMANDER_CORE_MODE_DIRECT:
            corsair->SetDirectColor(colors, zones);
            break;
        }
}
