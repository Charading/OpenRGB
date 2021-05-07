/*-----------------------------------------*\
|  RGBController_CorsairCapellix.cpp        |
|                                           |
|  Generic RGB Interface for Corsair        |
|  Capellix Series                          |
|                                           |
|  Jeff P.                                  |
\*-----------------------------------------*/

#include "RGBController_CorsairEliteCapellix.h"
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

static unsigned int matrix_map_ml_8led[5][5] =
     {{ 6,  NA,   7,  NA,   0},
      {NA,  NA,  NA,  NA,  NA},
      { 5,  NA,  NA,  NA,   1},
      {NA,  NA,  NA,  NA,  NA},
      { 4,  NA,   3,  NA,   2}};

static unsigned int matrix_map_ml_pro[3][3] =
     {{NA,  0,  NA},
      { 3,  NA,  1},
      {NA,  2,  NA}};

static unsigned int matrix_map_ql_front[7][7] =
     {{NA,  NA,   4,  NA,   5,  NA,  NA},
      {NA,  15,  NA,  NA,  NA,   6,  NA},
      {14,  NA,  NA,   0,  NA,  NA,   7},
      {NA,  NA,   3,  NA,   1,  NA,  NA},
      {13,  NA,  NA,   2,  NA,  NA,   8},
      {NA,  12,  NA,  NA,  NA,   9,  NA},
      {NA,  NA,  11,  NA,  10,  NA,  NA},
};

static unsigned int matrix_map_ql_back[7][7] =
     {{NA,  NA,  NA,   7,  NA,  NA,  NA},
      {NA,  NA,   6,  NA,   8,  NA,  NA},
      {NA,  17,   5,  NA,   0,   9,  NA},
      {16,  NA,   4,  NA,   1,  NA,  10},
      {NA,  15,   3,  NA,   2,  11,  NA},
      {NA,  NA,  14,  NA,  12,  NA,  NA},
      {NA,  NA,  NA,  13,  NA,  NA,  NA},
};

static unsigned int matrix_map_ll[7][7] =
     {{NA,  NA,  NA,  14,  NA,  NA,  NA},
      {NA,  NA,  15,  NA,  13,  NA,  NA},
      {NA,   4,  NA,   0,  NA,  12,  NA},
      { 5,  NA,   1,  NA,   3,  NA,  11},
      {NA,   6,  NA,   2,  NA,  10,  NA},
      {NA,  NA,   7,  NA,   9,  NA,  NA},
      {NA,  NA,  NA,   8,  NA,  NA,  NA},
};

static unsigned int matrix_map_hd[6][6] =
     {{NA,  NA,   2,   1,  NA,  NA},
      {NA,   3,  NA,  NA,   0,  NA},
      { 4,  NA,  NA,  NA,  NA,  11},
      { 5,  NA,  NA,  NA,  NA,  10},
      {NA,   6,  NA,  NA,   9,  NA},
      {NA,  NA,   7,   8,  NA,  NA},
};



RGBController_CorsairEliteCapellix::RGBController_CorsairEliteCapellix(CorsairEliteCapellixController* corsair_ptr)
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

RGBController_CorsairEliteCapellix::~RGBController_CorsairEliteCapellix()
{
    delete corsair;
}

void RGBController_CorsairEliteCapellix::SetupZones()
{
    std::vector<int> fanleds = corsair->DetectRGBFans();
    std::cout<<"Begin zone setup"<<std::endl;
    zone Fan1, Fan2, Fan3, Fan4, Fan5, Fan6, Fan7, Fan8, Fan9, Fan10, Fan11, Fan12;
    std::vector<zone> fanzones{Fan1, Fan2, Fan3, Fan4, Fan5, Fan6, Fan7, Fan8, Fan9, Fan10, Fan11, Fan12};
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
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_ml_8led;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
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
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_ml_pro;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
            break;

        case 34:
            std::cout<<"Adding 34 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans+1) + " Front";
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 16;
            fanzones[TotalFans].leds_max           = 16;
            fanzones[TotalFans].leds_count         = 16;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 7;
            fanzones[TotalFans].matrix_map->width  = 7;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_ql_front;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;

            std::cout<<"Adding 34 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans) + " Back";
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 18;
            fanzones[TotalFans].leds_max           = 18;
            fanzones[TotalFans].leds_count         = 18;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 7;
            fanzones[TotalFans].matrix_map->width  = 7;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_ql_back;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
            i++;
            break;

        case 16:
            std::cout<<"Adding 16 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans+1);
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 16;
            fanzones[TotalFans].leds_max           = 16;
            fanzones[TotalFans].leds_count         = 16;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 7;
            fanzones[TotalFans].matrix_map->width  = 7;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_ll;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
            break;

        case 12:
            std::cout<<"Adding 12 fan"<<std::endl;
            fanzones[TotalFans].name               = "Fan " + std::to_string(TotalFans+1);
            fanzones[TotalFans].type               = ZONE_TYPE_MATRIX;
            fanzones[TotalFans].leds_min           = 12;
            fanzones[TotalFans].leds_max           = 12;
            fanzones[TotalFans].leds_count         = 12;
            fanzones[TotalFans].matrix_map         = new matrix_map_type;
            fanzones[TotalFans].matrix_map->height = 6;
            fanzones[TotalFans].matrix_map->width  = 6;
            fanzones[TotalFans].matrix_map->map    = (unsigned int *)&matrix_map_hd;
            zones.push_back(fanzones[TotalFans]);
            TotalFans++;
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
    }
    std::cout<<"Zone setup done"<<std::endl;
    SetupColors();
}

void RGBController_CorsairEliteCapellix::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CorsairEliteCapellix::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_CorsairEliteCapellix::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairEliteCapellix::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CorsairEliteCapellix::SetCustomMode()
{
    active_mode = 0;
}

void RGBController_CorsairEliteCapellix::DeviceUpdateMode()
{
        switch(modes[active_mode].value)
        {
        case CORSAIR_CAPELLIX_MODE_DIRECT:
            corsair->SetDirectColor(colors);
            break;
        }
}
