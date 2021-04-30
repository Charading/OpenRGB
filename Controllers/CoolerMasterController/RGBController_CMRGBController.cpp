/*-------------------------------------------------------------------*\
|  RGBController_CMRGBController.cpp                                  |
|                                                                     |
|  Driver for Coolermaster RGB USB Controller                         |
|                                                                     |
|  Nic W (midgetspy)        13th Apr 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_CMRGBController.h"

RGBController_CMRGBController::RGBController_CMRGBController(CMRGBController *cmargb_ptr)
{
    cmargb                  = cmargb_ptr;

    name              = "Cooler Master RGB Controller";
    vendor            = "Cooler Master";
    type              = DEVICE_TYPE_LEDSTRIP;
    description       = cmargb->GetDeviceName();
    version           = "1.0";
    serial            = cmargb->GetSerial();
    location          = cmargb->GetLocation();

// There's really no reason to support this mode since it can just be done with the "Multiple" mode on the entire zone
//    mode Static;
//    Static.name          = "Static";
//    Static.value         = CM_RGBC_MODE_STATIC;
//    Static.flags         = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
//    Static.colors_min    = 1;
//    Static.colors_max    = 1;
//    Static.colors.resize(Static.colors_max);
//    Static.speed_min     = CM_RGBC_SPEED_SLOWEST;
//    Static.speed_max     = CM_RGBC_SPEED_FASTEST;
//    Static.color_mode    = MODE_COLORS_MODE_SPECIFIC;
//    modes.push_back(Static);

    mode Breathing;
    Breathing.name          = "Breathing";
    Breathing.value         = CM_RGBC_MODE_BREATHING;
    Breathing.flags         = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Breathing.colors_min    = 1;
    Breathing.colors_max    = 1;
    Breathing.colors.resize(Breathing.colors_max);
    Breathing.speed_min     = CM_RGBC_SPEED_SLOWEST;
    Breathing.speed_max     = CM_RGBC_SPEED_FASTEST;
    Breathing.color_mode    = MODE_COLORS_MODE_SPECIFIC;
    Breathing.speed         = CM_RGBC_SPEED_NORMAL;
    modes.push_back(Breathing);

    mode ColorCycle;
    ColorCycle.name         = "Color Cycle";
    ColorCycle.value        = CM_RGBC_MODE_COLOR_CYCLE;
    ColorCycle.flags        = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_RANDOM_COLOR;
    ColorCycle.speed_min    = CM_RGBC_SPEED_SLOWEST;
    ColorCycle.speed_max    = CM_RGBC_SPEED_FASTEST;
    ColorCycle.color_mode   = MODE_COLORS_RANDOM;
    ColorCycle.speed        = CM_RGBC_SPEED_NORMAL;
    modes.push_back(ColorCycle);

    mode Star;
    Star.name               = "Star";
    Star.value              = CM_RGBC_MODE_STAR;
    Star.flags              = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Star.colors_min         = 1;
    Star.colors_max         = 1;
    Star.colors.resize(Star.colors_max);
    Star.speed_min          = CM_RGBC_SPEED_SLOWEST;
    Star.speed_max          = CM_RGBC_SPEED_FASTEST;
    Star.color_mode         = MODE_COLORS_MODE_SPECIFIC;
    Star.speed              = CM_RGBC_SPEED_NORMAL;
    modes.push_back(Star);

    mode Multiple;
    Multiple.name           = "Multiple";
    Multiple.value          = CM_RGBC_MODE_MULTIPLE;
    Multiple.flags          = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Multiple.colors_min     = 1;
    Multiple.colors_max     = 1;
    Multiple.colors.resize(Multiple.colors_max);
    Multiple.color_mode     = MODE_COLORS_PER_LED;
    Multiple.speed          = 0;
    modes.push_back(Multiple);

    mode Off;
    Off.name                = "Turn Off";
    Off.value               = CM_RGBC_MODE_OFF;
    Off.color_mode          = MODE_COLORS_NONE;
    modes.push_back(Off);

    SetupZones();
}

RGBController_CMRGBController::~RGBController_CMRGBController()
{
    delete cmargb;
}


void RGBController_CMRGBController::SetupZones()
{
    /*-------------------------------------------------*\
    | Clear any existing color/LED configuration        |
    \*-------------------------------------------------*/
    leds.clear();
    zones.clear();
    colors.clear();

    zone* new_zone = new zone();

    new_zone->name          = "Controller";
    new_zone->type          = ZONE_TYPE_SINGLE;
    new_zone->leds_min      = 1;
    new_zone->leds_max      = 4;
    new_zone->leds_count    = 4;
    new_zone->matrix_map    = NULL;

    /*---------------------------------------------------------*\
    | One zone per port, one LED per zone                       |
    \*---------------------------------------------------------*/
    for(std::size_t i = 1; i <= CM_RGB_NUM_PORTS; i++)
    {
        led*  new_led  = new led();
        new_led->name  = "LED " + std::to_string(i);
        leds.push_back(*new_led);
    }

    zones.push_back(*new_zone);
    SetupColors();
}

void RGBController_CMRGBController::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CMRGBController::DeviceUpdateLEDs()
{
    for(size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        UpdateZoneLEDs(zone_idx);
    }
}

void RGBController_CMRGBController::UpdateZoneLEDs(int /*zone*/)
{
    //bool random_colours     = (modes[active_mode].color_mode == MODE_COLORS_RANDOM);

    //cmargb->SetLedsDirect(zones[zone].colors, random_colours);
}

void RGBController_CMRGBController::UpdateSingleLED(int /*led*/)
{
    //cmargb->SetMode( modes[active_mode].value, modes[active_mode].speed );
    //cmargb->SetLedsDirect( zones[0].colors, zones[0].leds_count );
}

void RGBController_CMRGBController::SetCustomMode()
{
    //active_mode = CM_SMALL_ARGB_MODE_DIRECT;  //The small ARGB may not support "Direct" mode
}

void RGBController_CMRGBController::DeviceUpdateMode()
{
    //bool random_colours     = (modes[active_mode].color_mode == MODE_COLORS_RANDOM);
    //RGBColor colour         = (modes[active_mode].color_mode == MODE_COLORS_MODE_SPECIFIC) ? modes[active_mode].colors[0] : 0;

    //cmargb->SetMode( modes[active_mode].value, modes[active_mode].speed, colour, random_colours );
}
