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

    mode Off;
    Off.name                = "Off";
    Off.value               = LOGITECH_G560_MODE_OFF;
    Off.flags               = 0;
    Off.color_mode          = MODE_COLORS_PER_LED;
    modes.push_back(Off);

    mode Static;
    Static.name             = "Static";
    Static.value            = LOGITECH_G560_MODE_STATIC;
    Static.flags            = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode       = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Cycle;
    Cycle.name              = "Cycle";
    Cycle.value             = LOGITECH_G560_MODE_CYCLE;
    Cycle.flags             = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Cycle.color_mode        = MODE_COLORS_PER_LED;
    Cycle.speed_min         = LOGITECH_G560_SPEED_SLOWEST;
    Cycle.speed_max         = LOGITECH_G560_SPEED_FASTEST;
    Cycle.speed             = LOGITECH_G560_SPEED_NORMAL;
    modes.push_back(Cycle);

    mode Breathing;
    Breathing.name          = "Breathing";
    Breathing.value         = LOGITECH_G560_MODE_BREATHING;
    Breathing.flags         = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Breathing.color_mode    = MODE_COLORS_PER_LED;
    Breathing.speed_min     = LOGITECH_G560_SPEED_SLOWEST;
    Breathing.speed_max     = LOGITECH_G560_SPEED_FASTEST;
    Breathing.speed         = LOGITECH_G560_SPEED_NORMAL;
    modes.push_back(Breathing);
    SetupZones();
}

void RGBController_LogitechG560::SetupZones()
{
    zone G560_left_front;
    G560_left_front.name           = "Left Front";
    G560_left_front.type           = ZONE_TYPE_SINGLE;
    G560_left_front.leds_min       = 1;
    G560_left_front.leds_max       = 1;
    G560_left_front.leds_count     = 1;
    G560_left_front.matrix_map     = NULL;
    zones.push_back(G560_left_front);

    led G560_left_front_led;
    G560_left_front_led.name = "Left Front";
    G560_left_front_led.value = 0x00;
    leds.push_back(G560_left_front_led);

    zone G560_right_front;
    G560_right_front.name           = "Right Front";
    G560_right_front.type           = ZONE_TYPE_SINGLE;
    G560_right_front.leds_min       = 1;
    G560_right_front.leds_max       = 1;
    G560_right_front.leds_count     = 1;
    G560_right_front.matrix_map     = NULL;
    zones.push_back(G560_right_front);

    led G560_right_front_led;
    G560_right_front_led.name = "Right Front";
    G560_right_front_led.value = 0x01;
    leds.push_back(G560_right_front_led);


    zone G560_left_rear;
    G560_left_rear.name           = "Left Rear";
    G560_left_rear.type           = ZONE_TYPE_SINGLE;
    G560_left_rear.leds_min       = 1;
    G560_left_rear.leds_max       = 1;
    G560_left_rear.leds_count     = 1;
    G560_left_rear.matrix_map     = NULL;
    zones.push_back(G560_left_rear);

    led G560_left_read_led;
    G560_left_read_led.name = "Left Rear";
    G560_left_read_led.value = 0x02;
    leds.push_back(G560_left_read_led);


    zone G560_right_rear;
    G560_right_rear.name           = "Right Rear";
    G560_right_rear.type           = ZONE_TYPE_SINGLE;
    G560_right_rear.leds_min       = 1;
    G560_right_rear.leds_max       = 1;
    G560_right_rear.leds_count     = 1;
    G560_right_rear.matrix_map     = NULL;
    zones.push_back(G560_right_rear);

    led G560_right_rear_led;
    G560_right_rear_led.name = "Right Rear";
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
