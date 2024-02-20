/*-----------------------------------------*\
|  RGBController_CorsairVoidPro.cpp         |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Corsair Void Pro Driver                  |
|                                           |
|  Guimard Morgan (morg) 2/08/2022          |
\*-----------------------------------------*/

#include "RGBController_CorsairVoidPro.h"

#include <thread>
#include <chrono>

RGBController_CorsairVoidPro::RGBController_CorsairVoidPro(CorsairVoidProController* controller_ptr)
{
    controller                          = controller_ptr;
    name                                = "Corsair Void Pro";
    vendor                              = "Corsair";
    type                                = DEVICE_TYPE_HEADSET;
    description                         = name;
    location                            = controller->GetDeviceLocation();
    serial                              = controller->GetSerialString();
    version                             = controller->GetFirmwareVersion();

    mode Direct;
    Direct.name                         = "Direct";
    Direct.value                        = DIRECT_MODE_VALUE;
    Direct.flags                        = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode                   = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_CorsairVoidPro::~RGBController_CorsairVoidPro()
{
    delete controller;
}

void RGBController_CorsairVoidPro::SetupZones()
{
    zone new_zone;

    new_zone.name       = "Headset";
    new_zone.type       = ZONE_TYPE_LINEAR;
    new_zone.leds_min   = CORSAIR_VOID_PRO_NUMBER_OF_LEDS;
    new_zone.leds_max   = CORSAIR_VOID_PRO_NUMBER_OF_LEDS;
    new_zone.leds_count = CORSAIR_VOID_PRO_NUMBER_OF_LEDS;
    new_zone.matrix_map = nullptr;

    zones.emplace_back(new_zone);

    leds.resize(new_zone.leds_count);

    for(unsigned int i = 0; i < CORSAIR_VOID_PRO_NUMBER_OF_LEDS; i++)
    {
        leds[i].name = "LED " + std::to_string(i + 1);
    }

    SetupColors();
}

void RGBController_CorsairVoidPro::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CorsairVoidPro::DeviceUpdateLEDs()
{
    UpdateZoneLEDs(0);
}

void RGBController_CorsairVoidPro::UpdateZoneLEDs(int /*zone*/)
{
    controller->SetDirect(colors);
}

void RGBController_CorsairVoidPro::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_CorsairVoidPro::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_CorsairVoidPro::DeviceUpdateMode()
{    
    UpdateZoneLEDs(0);
}
