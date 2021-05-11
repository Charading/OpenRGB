/*-------------------------------------------------------------------*\
|  RGBController_CMRGBController.cpp                                  |
|                                                                     |
|  Driver for Coolermaster RGB USB Controller                         |
|                                                                     |
|  Nic W (midgetspy)        13th Apr 2021                             |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_CMRGBController.h"

/*
 * This controller has 4 ports, each for a 12v non-addressable LED item.
 *
 * It supports the following modes:
 *   Static: All 4 ports a single color. Has brightness option.
 *   Breathing: All ports a single color, fading in and out. Has brightness and speed option.
 *   Star: Some weird effect using all 4 ports and a single color. Has brightness and speed option.
 *   Color Cycle: All ports cycle through the rainbow in unison. Has brightness and speed option.
 *   Off: All 4 ports off
 *
 * Plus some "special" modes:
 *   Multilayer: Each of the 4 ports can have any of the modes above applied individually
 *   Multiple Color/Customize: Each port can be set to its own static color
 *   Mirage: A strobe effect that varies the LED pulse frequency which affects any of the above modes
 *
 * Note:
 *  Multiple Color/Customize is equivalent to Multilayer + Static, but the device supports both separately
 *  Static is equivalent to Multiple Color/Customize with the same color on each port, but the device supports both separately
 *
 * It can be controlled with 2 different pieces of software: MasterPlus+ or "RGB LED Controller". They appear to use different protocols.
 *
 * RGB LED Controller:
 *   Sets changes temporarily and then applies them or cancels the changes separately
 *   Supports all modes above
 *   Has 3 brightness increments
 *   Has two different colors for the Star effect (Star/Sky)
 *
 * MasterPlus+:
 *   Sets changes permanently as soon as you change anything in the UI
 *   Doesn't support Multilayer or Mirage
 *   Has 5 brightness increments
 *   Single color for Star
 *
 */
RGBController_CMRGBController::RGBController_CMRGBController(CMRGBController *cmargb_ptr)
{
    cmrgb                  = cmargb_ptr;

    name                    = "Cooler Master RGB Controller";
    vendor                  = "Cooler Master";
    type                    = DEVICE_TYPE_LEDSTRIP;
    description             = cmrgb->GetDeviceName();
    version                 = "1.0";
    serial                  = cmrgb->GetSerial();
    location                = cmrgb->GetLocation();
    inCustomMode            = false;

    mode Static;
    Static.name             = "Static";
    Static.value            = CM_RGBC_MODE_STATIC;
    Static.flags            = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.colors_min       = 1;
    Static.colors_max       = 1;
    Static.colors.resize(Static.colors_max);
    Static.color_mode       = MODE_COLORS_MODE_SPECIFIC;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name          = "Breathing";
    Breathing.value         = CM_RGBC_MODE_BREATHING;
    Breathing.flags         = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Breathing.colors_min    = 1;
    Breathing.colors_max    = 1;
    Breathing.colors.resize(Breathing.colors_max);
    Breathing.speed_min     = CM_RGBC_SPEED_BREATHING_SLOWEST;
    Breathing.speed_max     = CM_RGBC_SPEED_BREATHING_FASTEST;
    Breathing.color_mode    = MODE_COLORS_MODE_SPECIFIC;
    Breathing.speed         = MidPoint(CM_RGBC_SPEED_BREATHING_SLOWEST, CM_RGBC_SPEED_BREATHING_FASTEST);
    modes.push_back(Breathing);

    mode ColorCycle;
    ColorCycle.name         = "Color Cycle";
    ColorCycle.value        = CM_RGBC_MODE_COLOR_CYCLE;
    ColorCycle.flags        = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_RANDOM_COLOR;
    ColorCycle.speed_min    = CM_RGBC_SPEED_COLOR_CYCLE_SLOWEST;
    ColorCycle.speed_max    = CM_RGBC_SPEED_COLOR_CYCLE_FASTEST;
    ColorCycle.color_mode   = MODE_COLORS_RANDOM;
    ColorCycle.speed        = MidPoint(CM_RGBC_SPEED_COLOR_CYCLE_SLOWEST, CM_RGBC_SPEED_COLOR_CYCLE_FASTEST);
    modes.push_back(ColorCycle);

    mode Star;
    Star.name               = "Star";
    Star.value              = CM_RGBC_MODE_STAR;
    Star.flags              = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Star.colors_min         = 1;
    Star.colors_max         = 1;
    Star.colors.resize(Star.colors_max);
    Star.speed_min          = CM_RGBC_SPEED_STAR_SLOWEST;
    Star.speed_max          = CM_RGBC_SPEED_STAR_FASTEST;
    Star.color_mode         = MODE_COLORS_MODE_SPECIFIC;
    Star.speed              = MidPoint(CM_RGBC_SPEED_STAR_SLOWEST, CM_RGBC_SPEED_STAR_FASTEST);
    modes.push_back(Star);

    mode Multiple;
    Multiple.name           = "Multiple Color";
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

    ReadAllModeConfigsFromDevice();
}

RGBController_CMRGBController::~RGBController_CMRGBController()
{
    delete cmrgb;
}

int RGBController_CMRGBController::MidPoint(int a, int b)
{
    int smallest    = a < b ? a : b;
    int biggest     = a > b ? a : b;

    return smallest + (biggest - smallest)/2;
}

void RGBController_CMRGBController::ReadAllModeConfigsFromDevice()
{
    int device_mode = cmrgb->GetMode();

    for(std::size_t mode_idx = 0; mode_idx < modes.size() ; mode_idx++)
    {
        if (device_mode == modes[mode_idx].value)
        {
            active_mode = mode_idx;
            continue;
        }

        if (!modes[mode_idx].flags)
        {
            continue;
        }

        cmrgb->ReadModeConfig(modes[mode_idx].value);
        LoadConfigFromDeviceController(mode_idx);
    }

    // leave the device config set to the active mode
    if (active_mode != -1)
    {
        cmrgb->ReadModeConfig(modes[active_mode].value);
        LoadConfigFromDeviceController(active_mode);
    }
}

void RGBController_CMRGBController::LoadConfigFromDeviceController(int mode_idx)
{
    if (modes[mode_idx].flags & MODE_FLAG_HAS_MODE_SPECIFIC_COLOR)
    {
        modes[mode_idx].colors[0] = cmrgb->GetModeColor();
    }

    if (modes[mode_idx].flags & MODE_FLAG_HAS_PER_LED_COLOR)
    {
        SetLED(0, cmrgb->GetPort1Color());
        SetLED(1, cmrgb->GetPort2Color());
        SetLED(2, cmrgb->GetPort3Color());
        SetLED(3, cmrgb->GetPort4Color());
    }

    if (modes[mode_idx].flags & MODE_FLAG_HAS_SPEED)
    {
        modes[mode_idx].speed = cmrgb->GetSpeed();
    }

    if (modes[mode_idx].flags & MODE_FLAG_HAS_BRIGHTNESS)
    {
        //TODO: hook up brightness once it's supported
        //modes[active_mode].brightness = cmargb->GetBrightness();
    }
}


void RGBController_CMRGBController::SetupZones()
{
    leds.clear();
    zones.clear();
    colors.clear();

    // One zone, 4 leds. This might not actually work with the Multilayer mode, but we'll deal with that later
    zone* new_zone          = new zone();
    new_zone->name          = "Controller";
    new_zone->type          = ZONE_TYPE_SINGLE;
    new_zone->leds_min      = 1;
    new_zone->leds_max      = 4;
    new_zone->leds_count    = 4;
    new_zone->matrix_map    = NULL;

    for(std::size_t i = 1; i <= CM_RGBC_NUM_PORTS; i++)
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
}

void RGBController_CMRGBController::DeviceUpdateLEDs()
{
    for(size_t zone_idx = 0; zone_idx < zones.size(); zone_idx++)
    {
        UpdateZoneLEDs(zone_idx);
    }
}

void RGBController_CMRGBController::UpdateZoneLEDs(int zone)
{
    cmrgb->SetLedsDirect(zones[zone].colors[0], zones[zone].colors[1], zones[zone].colors[2], zones[zone].colors[3], !inCustomMode);
}

void RGBController_CMRGBController::UpdateSingleLED(int /*led*/)
{
}

/*
 * This device doesn't have a direct mode per se, but every command can be optionally saved to flash.
 * So we'll just track a virtual direct/custom mode here and use that to decide if we should or shouldn't
 * be saving the commands to flash.
 */
void RGBController_CMRGBController::SetCustomMode()
{
    inCustomMode    = true;
}

void RGBController_CMRGBController::DeviceUpdateMode()
{
    inCustomMode    = false;

    if (cmrgb->GetMode() != modes[active_mode].value && modes[active_mode].flags & MODE_FLAG_HAS_PER_LED_COLOR)
    {
        LoadConfigFromDeviceController(active_mode);
    }

    RGBColor colour = (modes[active_mode].color_mode == MODE_COLORS_MODE_SPECIFIC) ? modes[active_mode].colors[0] : 0;

    // TODO: hook up brightness here
    unsigned char brightness = 0xFF;
    cmrgb->SetMode( modes[active_mode].value, modes[active_mode].speed, brightness, colour);
}
