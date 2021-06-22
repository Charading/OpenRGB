/*-----------------------------------------*\
|  RGBController_Sinowealth.cpp             |
|                                           |
|  Definitions and types for Sinowealth     |
|  mice, including Glorious                 |
|                                           |
|  Niels Westphal (crashniels) 20/5/2020    |
\*-----------------------------------------*/

#include "RGBController_SinowealthKeyboard.h"

RGBController_SinowealthKeyboard::RGBController_SinowealthKeyboard(SinowealthKeyboardController* sinowealth_ptr)
{
    sinowealth = sinowealth_ptr;

    name        = "Sinowealth Keyboard";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Sinowealth Keyboard";
    location    = sinowealth->GetLocation();
    serial      = sinowealth->GetSerialString();

    mode Static;
    Static.name       = "Static";
    Static.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode = MODE_COLORS_PER_LED;
    Static.value      = MODE_STATIC;
    modes.push_back(Static);

    mode Off;
    Off.name       = "Off";
    Off.flags      = 0;
    Off.color_mode = MODE_COLORS_NONE;
    Off.value      = MODE_OFF;
    modes.push_back(Off);

    mode Respire;
    Respire.name       = "Respire";
    Respire.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Respire.speed_min  = SPEED_SLOW;
    Respire.speed      = SPEED_NORMAL;
    Respire.speed_max  = SPEED_FASTEST;
    Respire.color_mode = MODE_COLORS_RANDOM;
    Respire.value      = MODE_RESPIRE;
    Respire.colors_min = 1;
    Respire.colors_max = 2;
    Respire.colors.resize(1);
    modes.push_back(Respire);

    mode Rainbow;
    Rainbow.name       = "Rainbow";
    Rainbow.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Rainbow.speed_min  = SPEED_SLOW;
    Rainbow.speed      = SPEED_NORMAL;
    Rainbow.speed_max  = SPEED_FASTEST;
    Rainbow.color_mode = MODE_COLORS_NONE;
    Rainbow.value      = MODE_RAINBOW;
    modes.push_back(Rainbow);

    mode FlashAway;
    FlashAway.name       = "Flash Away";
    FlashAway.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    FlashAway.speed_min  = SPEED_SLOW;
    FlashAway.speed      = SPEED_NORMAL;
    FlashAway.speed_max  = SPEED_FASTEST;
    FlashAway.color_mode = MODE_COLORS_RANDOM;
    FlashAway.value      = MODE_FLASH_AWAY;
    FlashAway.colors_min = 1;
    FlashAway.colors_max = 2;
    FlashAway.colors.resize(1);
    modes.push_back(FlashAway);

    mode Raindrops;
    Raindrops.name       = "Raindrops";
    Raindrops.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Raindrops.speed_min  = SPEED_SLOW;
    Raindrops.speed      = SPEED_NORMAL;
    Raindrops.speed_max  = SPEED_FASTEST;
    Raindrops.color_mode = MODE_COLORS_RANDOM;
    Raindrops.value      = MODE_RAINDROPS;
    Raindrops.colors_min = 1;
    Raindrops.colors_max = 2;
    Raindrops.colors.resize(1);
    modes.push_back(Raindrops);

    mode RainbowWheel;
    RainbowWheel.name       = "Rainbow Wheel";
    RainbowWheel.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RainbowWheel.speed_min  = SPEED_SLOW;
    RainbowWheel.speed      = SPEED_NORMAL;
    RainbowWheel.speed_max  = SPEED_FASTEST;
    RainbowWheel.color_mode = MODE_COLORS_RANDOM;
    RainbowWheel.value      = MODE_RAINBOW_WHEEL;
    RainbowWheel.colors_min = 1;
    RainbowWheel.colors_max = 2;
    RainbowWheel.colors.resize(1);
    modes.push_back(RainbowWheel);

    mode RipplesShining;
    RipplesShining.name       = "Ripples Shining";
    RipplesShining.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RipplesShining.speed_min  = SPEED_SLOW;
    RipplesShining.speed      = SPEED_NORMAL;
    RipplesShining.speed_max  = SPEED_FASTEST;
    RipplesShining.color_mode = MODE_COLORS_RANDOM;
    RipplesShining.value      = MODE_RIPPLES_SHINING;
    RipplesShining.colors_min = 1;
    RipplesShining.colors_max = 2;
    RipplesShining.colors.resize(1);
    modes.push_back(RipplesShining);

    mode StarsTwinkle;
    StarsTwinkle.name       = "Stars Twinkle";
    StarsTwinkle.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    StarsTwinkle.speed_min  = SPEED_SLOW;
    StarsTwinkle.speed      = SPEED_NORMAL;
    StarsTwinkle.speed_max  = SPEED_FASTEST;
    StarsTwinkle.color_mode = MODE_COLORS_RANDOM;
    StarsTwinkle.value      = MODE_STARS_TWINKLE;
    StarsTwinkle.colors_min = 1;
    StarsTwinkle.colors_max = 2;
    StarsTwinkle.colors.resize(1);
    modes.push_back(StarsTwinkle);

    mode ShadowDisappear;
    ShadowDisappear.name       = "Shadow Disappear";
    ShadowDisappear.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    ShadowDisappear.speed_min  = SPEED_SLOW;
    ShadowDisappear.speed      = SPEED_NORMAL;
    ShadowDisappear.speed_max  = SPEED_FASTEST;
    ShadowDisappear.color_mode = MODE_COLORS_RANDOM;
    ShadowDisappear.value      = MODE_SHADOW_DISAPPEAR;
    ShadowDisappear.colors_min = 1;
    ShadowDisappear.colors_max = 2;
    ShadowDisappear.colors.resize(1);
    modes.push_back(ShadowDisappear);

    mode RetroSnake;
    RetroSnake.name       = "Retro Snake";
    RetroSnake.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RetroSnake.speed_min  = SPEED_SLOW;
    RetroSnake.speed      = SPEED_NORMAL;
    RetroSnake.speed_max  = SPEED_FASTEST;
    RetroSnake.color_mode = MODE_COLORS_RANDOM;
    RetroSnake.value      = MODE_RETRO_SNAKE;
    RetroSnake.colors_min = 1;
    RetroSnake.colors_max = 2;
    RetroSnake.colors.resize(1);
    modes.push_back(RetroSnake);

    mode NeonStream;
    NeonStream.name       = "Neon Stream";
    NeonStream.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    NeonStream.speed_min  = SPEED_SLOW;
    NeonStream.speed      = SPEED_NORMAL;
    NeonStream.speed_max  = SPEED_FASTEST;
    NeonStream.color_mode = MODE_COLORS_RANDOM;
    NeonStream.value      = MODE_NEON_STREAM;
    NeonStream.colors_min = 1;
    NeonStream.colors_max = 2;
    NeonStream.colors.resize(1);
    modes.push_back(NeonStream);

    mode Reaction;
    Reaction.name       = "Reaction";
    Reaction.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Reaction.speed_min  = SPEED_SLOW;
    Reaction.speed      = SPEED_NORMAL;
    Reaction.speed_max  = SPEED_FASTEST;
    Reaction.color_mode = MODE_COLORS_RANDOM;
    Reaction.value      = MODE_REACTION;
    Reaction.colors_min = 1;
    Reaction.colors_max = 2;
    Reaction.colors.resize(1);
    modes.push_back(Reaction);

    mode SineWave;
    SineWave.name       = "Sine Wave";
    SineWave.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    SineWave.speed_min  = SPEED_SLOW;
    SineWave.speed      = SPEED_NORMAL;
    SineWave.speed_max  = SPEED_FASTEST;
    SineWave.color_mode = MODE_COLORS_RANDOM;
    SineWave.value      = MODE_SINE_WAVE;
    SineWave.colors_min = 1;
    SineWave.colors_max = 2;
    SineWave.colors.resize(1);
    modes.push_back(SineWave);

    mode RetinueScanning;
    RetinueScanning.name       = "Retinue Scanning";
    RetinueScanning.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RetinueScanning.speed_min  = SPEED_SLOW;
    RetinueScanning.speed      = SPEED_NORMAL;
    RetinueScanning.speed_max  = SPEED_FASTEST;
    RetinueScanning.color_mode = MODE_COLORS_RANDOM;
    RetinueScanning.value      = MODE_RETINUE_SCANNING;
    RetinueScanning.colors_min = 1;
    RetinueScanning.colors_max = 2;
    RetinueScanning.colors.resize(1);
    modes.push_back(RetinueScanning);

    mode RotatingWindmill;
    RotatingWindmill.name       = "Rotating Windmill";
    RotatingWindmill.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RotatingWindmill.speed_min  = SPEED_SLOW;
    RotatingWindmill.speed      = SPEED_NORMAL;
    RotatingWindmill.speed_max  = SPEED_FASTEST;
    RotatingWindmill.color_mode = MODE_COLORS_RANDOM;
    RotatingWindmill.value      = MODE_ROTATING_WINDMILL;
    RotatingWindmill.colors_min = 1;
    RotatingWindmill.colors_max = 2;
    RotatingWindmill.colors.resize(1);
    modes.push_back(RotatingWindmill);

    mode ColorfulWaterfall;
    ColorfulWaterfall.name       = "Colorful Waterfall";
    ColorfulWaterfall.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    ColorfulWaterfall.speed_min  = SPEED_SLOW;
    ColorfulWaterfall.speed      = SPEED_NORMAL;
    ColorfulWaterfall.speed_max  = SPEED_FASTEST;
    ColorfulWaterfall.color_mode = MODE_COLORS_NONE;
    ColorfulWaterfall.value      = MODE_COLORFUL_WATERFALL;
    modes.push_back(ColorfulWaterfall);

    mode Blossoming;
    Blossoming.name       = "Blossoming";
    Blossoming.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS;
    Blossoming.speed_min  = SPEED_SLOW;
    Blossoming.speed      = SPEED_NORMAL;
    Blossoming.speed_max  = SPEED_FASTEST;
    Blossoming.color_mode = MODE_COLORS_NONE;
    Blossoming.value      = MODE_BLOSSOMING;
    modes.push_back(Blossoming);

    mode RotatingStorm;
    RotatingStorm.name       = "Rotating Storm";
    RotatingStorm.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    RotatingStorm.speed_min  = SPEED_SLOW;
    RotatingStorm.speed      = SPEED_NORMAL;
    RotatingStorm.speed_max  = SPEED_FASTEST;
    RotatingStorm.color_mode = MODE_COLORS_RANDOM;
    RotatingStorm.value      = MODE_ROTATING_STORM;
    RotatingStorm.colors_min = 1;
    RotatingStorm.colors_max = 2;
    RotatingStorm.colors.resize(1);
    modes.push_back(RotatingStorm);

    mode Collision;
    Collision.name       = "Collision";
    Collision.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Collision.speed_min  = SPEED_SLOW;
    Collision.speed      = SPEED_NORMAL;
    Collision.speed_max  = SPEED_FASTEST;
    Collision.color_mode = MODE_COLORS_RANDOM;
    Collision.value      = MODE_COLLISION;
    Collision.colors_min = 1;
    Collision.colors_max = 2;
    Collision.colors.resize(1);
    modes.push_back(Collision);

    mode Perfect;
    Perfect.name       = "Perfect";
    Perfect.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Perfect.speed_min  = SPEED_SLOW;
    Perfect.speed      = SPEED_NORMAL;
    Perfect.speed_max  = SPEED_FASTEST;
    Perfect.color_mode = MODE_COLORS_RANDOM;
    Perfect.value      = MODE_PERFECT;
    Perfect.colors_min = 1;
    Perfect.colors_max = 2;
    Perfect.colors.resize(1);
    modes.push_back(Perfect);

    mode PerKey;
    PerKey.name       = "Per Key";
    PerKey.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    PerKey.color_mode = MODE_COLORS_PER_LED;
    PerKey.value      = MODE_PER_KEY;
    modes.push_back(PerKey);


    SetupZones();
}

RGBController_SinowealthKeyboard::~RGBController_SinowealthKeyboard()
{
    delete sinowealth;
}

void RGBController_SinowealthKeyboard::SetupZones()
{
    /*---------------------------------------------------------*\
    | Create a single zone                                      |
    \*---------------------------------------------------------*/
    zone new_zone;
    new_zone.name       = "Keyboard";
    new_zone.type       = ZONE_TYPE_MATRIX;
    new_zone.leds_min   = sinowealth->GetLEDCount();
    new_zone.leds_max   = sinowealth->GetLEDCount();
    new_zone.leds_count = sinowealth->GetLEDCount();
    new_zone.matrix_map = NULL;
    zones.push_back(new_zone);

    /*---------------------------------------------------------*\
    | Set up LEDs                                               |
    \*---------------------------------------------------------*/
    for(std::size_t led_idx = 0; led_idx < zones[0].leds_count; led_idx++)
    {
        led* new_led = new led();
        new_led->name = "Mouse LED";
        leds.push_back(*new_led);
    }

    SetupColors();
}

void RGBController_SinowealthKeyboard::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SinowealthKeyboard::DeviceUpdateLEDs()
{
    sinowealth->SetLEDColor(&colors[0]);
}

void RGBController_SinowealthKeyboard::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SinowealthKeyboard::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SinowealthKeyboard::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_SinowealthKeyboard::DeviceUpdateMode()

{
    unsigned int speed = SPEED_NORMAL;
    unsigned int brightness = BRIGHTNESS_FULL;

    if (modes[active_mode].value == MODE_STATIC)
    {

    }
    else
    {

        auto color_mode = modes[active_mode].color_mode == MODE_COLORS_NONE ? 0 : &modes[active_mode].colors[0];
        //auto active_mode_check = modes[active_mode];
        //auto test = &modes[active_mode].colors;
        sinowealth->SetMode(modes[active_mode].value, brightness, modes[active_mode].speed, modes[active_mode].color_mode, color_mode);
    }
}
