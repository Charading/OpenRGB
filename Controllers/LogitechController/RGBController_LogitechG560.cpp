/*-----------------------------------------*\
|  RGBController_LogitechG560.cpp           |
|                                           |
|  Generic RGB Interface for                |
|  Logitech G560 RGB Speaker                |
|                                           |
|  Cheerpipe     10/28/2020                 |
|  Based on                                 |
|  TheRogueZeta   8/31/2020                 |
\*-----------------------------------------*/

#include "RGBController_LogitechG560.h"

RGBController_LogitechG560::RGBController_LogitechG560(LogitechG560Controller* logitech_ptr)
{
    logitech = logitech_ptr;

    name        = "Logitech G560 RGB Speaker";
    type        = DEVICE_TYPE_SPEAKER;
    description = "Logitech G560 RGB Speaker";

    mode Direct;
    Direct.name             = "Direct";
    Direct.value            = LOGITECH_G560_MODE_DIRECT;
    Direct.flags            = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode       = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Off;
    Off.name                = "Off";
    Off.value               = LOGITECH_G560_MODE_OFF;
    Off.flags               = 0;
    Off.color_mode          = MODE_COLORS_PER_LED;
    modes.push_back(Off);

    SetupZones();
}

void RGBController_LogitechG560::SetupZones()
{
    zone G60_speaker_left;
    G60_speaker_left.name           = "Left speaker";
    G60_speaker_left.type           = ZONE_TYPE_LINEAR;
    G60_speaker_left.leds_min       = 2;
    G60_speaker_left.leds_max       = 2;
    G60_speaker_left.leds_count     = 2;
    G60_speaker_left.matrix_map     = NULL;
    zones.push_back(G60_speaker_left);

    led G560_left_front_led;
    G560_left_front_led.name = "Front left";
    G560_left_front_led.value = 0x00;
    leds.push_back(G560_left_front_led);

    led G560_left_rear_led;
    G560_left_rear_led.name = "Rear left";
    G560_left_rear_led.value = 0x02;
    leds.push_back(G560_left_rear_led);

    zone G60_speaker_right;
    G60_speaker_right.name           = "Right speaker";
    G60_speaker_right.type           = ZONE_TYPE_LINEAR;
    G60_speaker_right.leds_min       = 2;
    G60_speaker_right.leds_max       = 2;
    G60_speaker_right.leds_count     = 2;
    G60_speaker_right.matrix_map     = NULL;
    zones.push_back(G60_speaker_right);

    led G560_right_front_led;
    G560_right_front_led.name = "Front right";
    G560_right_front_led.value = 0x01;
    leds.push_back(G560_right_front_led);

    led G560_right_rear_led;
    G560_right_rear_led.name = "Rear right";
    G560_right_rear_led.value = 0x03;
    leds.push_back(G560_right_rear_led);

    SetupColors();
}

void RGBController_LogitechG560::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_LogitechG560::DeviceUpdateLEDs()
{
    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        unsigned char red = RGBGetRValue(colors[led_idx]);
        unsigned char grn = RGBGetGValue(colors[led_idx]);
        unsigned char blu = RGBGetBValue(colors[led_idx]);

        logitech->SendSpeakerMode((unsigned char)leds[led_idx].value, modes[active_mode].value,red, grn, blu);
    }
}

void RGBController_LogitechG560::UpdateZoneLEDs(int zone)
{

}

void RGBController_LogitechG560::UpdateSingleLED(int led)
{

}

void RGBController_LogitechG560::SetCustomMode()
{

}

void RGBController_LogitechG560::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
