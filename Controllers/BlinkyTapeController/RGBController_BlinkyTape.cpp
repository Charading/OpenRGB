/*---------------------------------------------------------*\
|  RGBController_BlinkyTape.cpp                             |
|                                                           |
|  Generic RGB Interface for BlinkyTape Led controller      |
|                                                           |
|  Matt Mets (matt@blinkinlabs.com), 07/01/2021             |
\*---------------------------------------------------------*/

#include "RGBController_BlinkyTape.h"


RGBController_BlinkyTape::RGBController_BlinkyTape(BlinkyTapeController* blinkytape_ptr)
{
    blinkytape = blinkytape_ptr;

    name        = "BlinkyTape";
    vendor      = "Blinkinlabs";
    type        = DEVICE_TYPE_LEDSTRIP;
    description = "USB Digital LED Controller";
    location    = blinkytape->GetLocation();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_BlinkyTape::~RGBController_BlinkyTape()
{
    delete blinkytape;
}

void RGBController_BlinkyTape::SetupZones()
{
    zone led_zone;
    led_zone.name       = "LED Strip";
    led_zone.type       = ZONE_TYPE_LINEAR;
    led_zone.leds_min   = blinkytape->num_leds;
    led_zone.leds_max   = blinkytape->num_leds;
    led_zone.leds_count = blinkytape->num_leds;
    led_zone.matrix_map = NULL;
    zones.push_back(led_zone);

    for(int led_idx = 0; led_idx < blinkytape->num_leds; led_idx++)
    {
        led new_led;
        new_led.name = "LED ";
        new_led.name.append(std::to_string(led_idx));

        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_BlinkyTape::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_BlinkyTape::DeviceUpdateLEDs()
{
    blinkytape->SetLEDs(colors);
}

void RGBController_BlinkyTape::UpdateZoneLEDs(int /*zone*/)
{
    blinkytape->SetLEDs(colors);
}

void RGBController_BlinkyTape::UpdateSingleLED(int /*led*/)
{
    blinkytape->SetLEDs(colors);
}

void RGBController_BlinkyTape::SetCustomMode()
{

}

void RGBController_BlinkyTape::DeviceUpdateMode()
{

}
