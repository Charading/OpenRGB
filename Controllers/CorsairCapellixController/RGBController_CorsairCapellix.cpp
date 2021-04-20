/*-----------------------------------------*\
|  RGBController_CorsairCapellix.cpp        |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Capellix Series                          |
|  Based on code by:                        |
|  Adam Honse (CalcProgrammer1) 8/18/2020   |
\*-----------------------------------------*/

#include "RGBController_CorsairCapellix.h"
#include <iostream>
RGBController_CorsairCapellix::RGBController_CorsairCapellix(CorsairCapellixController* corsair_ptr)
{
    corsair = corsair_ptr;

    vendor      = "Corsair";
    description = "Corsair Capellix Series Device";
    type        = DEVICE_TYPE_COOLER;
    //location    = corsair->GetLocation();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Static;
    Static.name       = "Static";
    Static.value      = 1;
    Static.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Static.colors_min = 1;
    Static.colors_max = 1;
    Static.colors.resize(1);
    modes.push_back(Static);

    mode RainbowWave;
    RainbowWave.name       = "Rainbow Wave";
    RainbowWave.value      = 2;
    RainbowWave.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    RainbowWave.color_mode = MODE_COLORS_NONE;
    RainbowWave.speed_min  = 0x03;
    RainbowWave.speed_max  = 0x05;
    RainbowWave.speed      = 0x04;
    modes.push_back(RainbowWave);

    mode SpiralRainbow;
    SpiralRainbow.name       = "Spiral Rainbow";
    SpiralRainbow.value      = 3;
    SpiralRainbow.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    SpiralRainbow.color_mode = MODE_COLORS_NONE;
    SpiralRainbow.speed_min  = 0x03;
    SpiralRainbow.speed_max  = 0x05;
    SpiralRainbow.speed      = 0x04;
    modes.push_back(SpiralRainbow);

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.value      = 4;
    Rainbow.flags      = MODE_FLAG_HAS_SPEED;
    Rainbow.color_mode = MODE_COLORS_NONE;
    Rainbow.speed_min  = 0x03;
    Rainbow.speed_max  = 0x05;
    Rainbow.speed      = 0x04;
    modes.push_back(Rainbow);

    mode ColorShift;
    ColorShift.name       = "Color Shift";
    ColorShift.value      = 5;
    ColorShift.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED;
    ColorShift.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    ColorShift.speed_min  = 0x03;
    ColorShift.speed_max  = 0x05;
    ColorShift.speed      = 0x04;
    ColorShift.colors_min = 2;
    ColorShift.colors_max = 2;
    ColorShift.colors.resize(2);
    modes.push_back(ColorShift);

    SetupZones();
}

RGBController_CorsairCapellix::~RGBController_CorsairCapellix()
{
    delete corsair;
}

void RGBController_CorsairCapellix::SetupZones()
{
    zone new_zone;

    new_zone.name       = "Pump Zone";
    new_zone.type       = ZONE_TYPE_SINGLE;
    new_zone.leds_min   = 1;
    new_zone.leds_max   = 1;
    new_zone.leds_count = 1;
    new_zone.matrix_map = NULL;
    zones.push_back(new_zone);

    led new_led;

    new_led.name        = "Pump LED";
    leds.push_back(new_led);
    SetupColors();
}

void RGBController_CorsairCapellix::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CorsairCapellix::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_CorsairCapellix::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairCapellix::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairCapellix::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_CorsairCapellix::DeviceUpdateMode()
{
        RGBColor      color = colors[0];
        unsigned char red   = RGBGetRValue(color);
        unsigned char grn   = RGBGetGValue(color);
        unsigned char blu   = RGBGetBValue(color);

        switch(modes[active_mode].value)
        {
        case CORSAIR_CAPELLIX_MODE_DIRECT:
            corsair->StartKeepaliveThread();
            corsair->SetDirectColor(red, grn, blu);
            break;

        case CORSAIR_CAPELLIX_MODE_STATIC:
            corsair->PauseKeepaliveThread();
            corsair->SetStatic(modes[active_mode].colors);
            break;

        case CORSAIR_CAPELLIX_MODE_RAINBOWWAVE:
            corsair->PauseKeepaliveThread();
            corsair->SetRainbowWave(modes[active_mode].speed, modes[active_mode].direction);
            break;

        case CORSAIR_CAPELLIX_MODE_SPIRALRAINBOW:
            corsair->PauseKeepaliveThread();
            corsair->SetSpiralRainbow(modes[active_mode].speed, modes[active_mode].direction);
            break;

        case CORSAIR_CAPELLIX_MODE_RAINBOW:
            corsair->PauseKeepaliveThread();
            corsair->SetRainbow(modes[active_mode].speed);
            break;

        case CORSAIR_CAPELLIX_MODE_COLORSHIFT:
            corsair->PauseKeepaliveThread();
            corsair->SetColorShift(modes[active_mode].speed, modes[active_mode].color_mode, modes[active_mode].colors);
            break;

        default:
            corsair->PauseKeepaliveThread();
        break;

        }

}
