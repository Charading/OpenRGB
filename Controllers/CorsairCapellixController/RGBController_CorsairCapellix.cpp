/*-----------------------------------------*\
|  RGBController_CorsairCapellix.cpp        |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Capellix Series                          |
|                                           |
|  Jeff P.                                  |
\*-----------------------------------------*/

#include "RGBController_CorsairCapellix.h"
#include <iostream>

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

static unsigned int matrix_map_pump[7][7] =
     {{28,  NA,  27,  NA,  26,  NA,  25},
      {NA,  16,  NA,  15,  NA,  14,  NA},
      {17,  NA,   0,   5,   3,  NA,  24},
      {NA,   9,   4,   8,   6,  13,  NA},
      {18,  NA,   1,   7,   2,  NA,  23},
      {NA,  10,  NA,  11,  NA,  12,  NA},
      {19,  NA,  20,  NA,  21,  NA,  22},
};

/*----------------------------------------*/
//Larger but more accurate pump matrix map//
/*
static unsigned int matrix_map_pump[9][7] =
     {{28,  NA,  27,  NA,  26,  NA,  25},
      {NA,  16,  NA,  15,  NA,  14,  NA},
      {NA,  NA,   0,  NA,   3,  NA,  NA},
      {17,  NA,  NA,   5,  NA,  NA,  24},
      {NA,   9,   4,   8,   6,  13,  NA},
      {18,  NA,  NA,   7,  NA,  NA,  23},
      {NA,  NA,   1,  NA,   2,  NA,  NA},
      {NA,  10,  NA,  11,  NA,  12,  NA},
      {19,  NA,  20,  NA,  21,  NA,  22},
};
*/

/*---------------------------------*/
//Smaller but worse pump matrix map//
/*
static unsigned int matrix_map_pump[7][5] =
     {{28,  27,  15,  26,  25},
      {16,   0,  NA,   3,  14},
      {17,  NA,   5,  NA,  24},
      { 9,   4,   8,   6,  13},
      {18,  NA,   7,  NA,  23},
      {10,   1,  NA,   2,  12},
      {19,  20,  11,  21,  22}};
*/

static unsigned int matrix_map_8ledfan[5][5] =
     {{ 6,  NA,   7,  NA,   0},
      {NA,  NA,  NA,  NA,  NA},
      { 5,  NA,  NA,  NA,   1},
      {NA,  NA,  NA,  NA,  NA},
      { 4,  NA,   3,  NA,   2}};

static unsigned int matrix_map_4ledfan[3][3] =
     {{NA,  0,  NA},
      { 3,  NA,  1},
      {NA,  2,  NA}};

static unsigned int matrix_map_ql_front[7][7] =
     {{NA,  NA,  NA,  NA,  NA,  NA,  NA},
      {NA,  NA,  NA,  NA,  NA,  NA,  NA},
      {NA,  NA,  NA,   0,  NA,  NA,  NA},
      {NA,  NA,   3,  NA,   1,  NA,  NA},
      {NA,  NA,  NA,   2,  NA,  NA,  NA},
      {NA,  NA,  NA,  NA,  NA,  NA,  NA},
      {NA,  NA,  NA,  NA,  NA,  NA,  NA},
};

RGBController_CorsairCapellix::RGBController_CorsairCapellix(CorsairCapellixController* corsair_ptr)
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

    mode ColorPulse;
    ColorPulse.name       = "Color Pulse";
    ColorPulse.value      = 6;
    ColorPulse.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED;
    ColorPulse.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    ColorPulse.speed_min  = 0x03;
    ColorPulse.speed_max  = 0x05;
    ColorPulse.speed      = 0x04;
    ColorPulse.colors_min = 2;
    ColorPulse.colors_max = 2;
    ColorPulse.colors.resize(2);
    modes.push_back(ColorPulse);

    mode ColorWave;
    ColorWave.name       = "Color Wave";
    ColorWave.value      = 7;
    ColorWave.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    ColorWave.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    ColorWave.speed_min  = 0x03;
    ColorWave.speed_max  = 0x05;
    ColorWave.speed      = 0x04;
    ColorWave.colors_min = 2;
    ColorWave.colors_max = 2;
    ColorWave.colors.resize(2);
    modes.push_back(ColorWave);

    mode Sequential;
    Sequential.name       = "Sequential";
    Sequential.value      = 8;
    Sequential.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    Sequential.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Sequential.speed_min  = 0x03;
    Sequential.speed_max  = 0x05;
    Sequential.speed      = 0x04;
    Sequential.colors_min = 1;
    Sequential.colors_max = 1;
    Sequential.colors.resize(1);
    modes.push_back(Sequential);

    mode Strobing;
    Strobing.name       = "Strobing";
    Strobing.value      = 9;
    Strobing.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED;
    Strobing.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Strobing.speed_min  = 0x03;
    Strobing.speed_max  = 0x05;
    Strobing.speed      = 0x04;
    Strobing.colors_min = 2;
    Strobing.colors_max = 2;
    Strobing.colors.resize(2);
    modes.push_back(Strobing);

    mode Visor;
    Visor.name       = "Visor";
    Visor.value      = 10;
    Visor.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED;
    Visor.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Visor.speed_min  = 0x03;
    Visor.speed_max  = 0x05;
    Visor.speed      = 0x04;
    Visor.colors_min = 2;
    Visor.colors_max = 2;
    Visor.colors.resize(2);
    modes.push_back(Visor);

    mode Marquee;
    Marquee.name       = "Marquee";
    Marquee.value      = 11;
    Marquee.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED;
    Marquee.color_mode = MODE_COLORS_MODE_SPECIFIC;
    Marquee.speed_min  = 0x03;
    Marquee.speed_max  = 0x05;
    Marquee.speed      = 0x04;
    Marquee.colors_min = 1;
    Marquee.colors_max = 1;
    Marquee.colors.resize(1);
    modes.push_back(Marquee);

    mode Rain;
    Rain.name       = "Rain";
    Rain.value      = 12;
    Rain.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED;
    Rain.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Rain.speed_min  = 0x03;
    Rain.speed_max  = 0x05;
    Rain.speed      = 0x04;
    Rain.colors_min = 2;
    Rain.colors_max = 2;
    Rain.colors.resize(2);
    modes.push_back(Rain);

    mode ColorWarp;
    ColorWarp.name       = "Color Warp";
    ColorWarp.value      = 13;
    ColorWarp.flags      = MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    ColorWarp.color_mode = MODE_COLORS_RANDOM;
    ColorWarp.speed_min  = 0x03;
    ColorWarp.speed_max  = 0x05;
    ColorWarp.speed      = 0x04;
    modes.push_back(ColorWarp);

    mode Arc;
    Arc.name       = "Arc";
    Arc.value      = 14;
    Arc.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    Arc.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Arc.speed_min  = 0x03;
    Arc.speed_max  = 0x05;
    Arc.speed      = 0x04;
    Arc.colors_min = 2;
    Arc.colors_max = 2;
    Arc.colors.resize(2);
    modes.push_back(Arc);

    mode Heartbeat;
    Heartbeat.name       = "Heartbeat";
    Heartbeat.value      = 15;
    Heartbeat.flags      = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR;
    Heartbeat.color_mode = MODE_COLORS_MODE_SPECIFIC | MODE_COLORS_RANDOM;
    Heartbeat.speed_min  = 0x03;
    Heartbeat.speed_max  = 0x05;
    Heartbeat.speed      = 0x04;
    Heartbeat.colors_min = 2;
    Heartbeat.colors_max = 2;
    Heartbeat.colors.resize(2);
    modes.push_back(Heartbeat);
}

RGBController_CorsairCapellix::~RGBController_CorsairCapellix()
{
    delete corsair;
}

void RGBController_CorsairCapellix::SetupZones()
{
    std::vector<int> fanleds = corsair->DetectFans();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout<<"Begin zone setup"<<std::endl;
    zone Fan1, Fan2, Fan3, Fan4, Fan5, Fan6;
    std::vector<zone> fanzones{Fan1, Fan2, Fan3, Fan4, Fan5, Fan6};
    int TotalFans=0;
    std::cout<<"Adding pump zone"<<std::endl;
    zone CapellixPump;
    CapellixPump.name               = "Pump";
    CapellixPump.type               = ZONE_TYPE_MATRIX;
    CapellixPump.leds_min           = 29;
    CapellixPump.leds_max           = 29;
    CapellixPump.leds_count         = 29;
    CapellixPump.matrix_map         = new matrix_map_type;
    CapellixPump.matrix_map->height = 7;
    CapellixPump.matrix_map->width  = 7;
    CapellixPump.matrix_map->map    = (unsigned int *)&matrix_map_pump;
    zones.push_back(CapellixPump);
    std::cout<<"Added pump zone"<<std::endl;

    for(int i = 0; i<6; i++){
        switch(fanleds[i]){
        case 8:
            std::cout<<"Adding 8 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans+1);
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 8;
            fanzones[TotalFans].leds_max           = 8;
            fanzones[TotalFans].leds_count         = 8;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 5;
            fanzones[TotalFans].matrix_map->width  = 5;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_8ledfan;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
            std::cout<<"Added 8 fan"<<std::endl;
            break;

        case 4:
            std::cout<<"Adding 4 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans+1);
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 4;
            fanzones[TotalFans].leds_max           = 4;
            fanzones[TotalFans].leds_count         = 4;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 3;
            fanzones[TotalFans].matrix_map->width  = 3;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_4ledfan;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
            std::cout<<"Added 4 fan"<<std::endl;
            break;
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
        std::cout<<"Next"<<std::endl;

    }
    std::cout<<"Zone setup done"<<std::endl;

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
        switch(modes[active_mode].value)
        {
        case CORSAIR_CAPELLIX_MODE_DIRECT:
            corsair->RestartKeepaliveThread();
            corsair->SetDirectColor(colors);
            break;
        default:
            corsair->SendHWMode(modes[active_mode].value, modes[active_mode].speed, modes[active_mode].direction, modes[active_mode].color_mode, modes[active_mode].colors);
            break;
        }
}
