/*-----------------------------------------*\
|  RGBController_TrustGXT970.cpp            |
|                                           |
|  Generic RGB Interface for OpenRGB        |
|  Trust GXT 970 USB Driver                 |
|                                           |
|  David Heidelberg  12/31/2022             |
\*-----------------------------------------*/

#include "RGBController_TrustGXT970.h"

#include <thread>
#include <chrono>

/**------------------------------------------------------------------*\
    @name Trust GXT 970
    @category Mouse
    @type USB
    @save :x:
    @direct :x:
    @effects :white_check_mark:
    @detectors DetectTrustGXT970Controllers
    @comment
\*-------------------------------------------------------------------*/

RGBController_TrustGXT970::RGBController_TrustGXT970(TrustGXT970Controller* controller_ptr)
{
    controller                          = controller_ptr;
    name                                = "Trust GXT 970";
    vendor                              = "Trust";
    type                                = DEVICE_TYPE_MOUSE;
    description                         = name;
    location                            = controller->GetDeviceLocation();
    serial                              = controller->GetSerialString();
    version                             = controller->GetFirmwareVersion();

    mode Static;
    Static.name                         = "Static";
    Static.value                        = TRUST_GXT_970_STATIC_MODE_VALUE;
    Static.flags                        = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Static.color_mode                   = MODE_COLORS_PER_LED;
    Static.brightness_min               = TRUST_GXT_970_BRIGHTNESS_MIN;
    Static.brightness_max               = TRUST_GXT_970_BRIGHTNESS_MAX;
    Static.brightness                   = TRUST_GXT_970_BRIGHTNESS_MAX;
    modes.push_back(Static);

    mode Breathing;
    Breathing.name                      = "Breathing";
    Breathing.value                     = TRUST_GXT_970_BREATHING_MODE_VALUE;
    Breathing.flags                     = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Breathing.color_mode                = MODE_COLORS_PER_LED;
    Breathing.speed_min                 = TRUST_GXT_970_SPEED_MIN;
    Breathing.speed_max                 = TRUST_GXT_970_SPEED_MAX;
    Breathing.speed                     = TRUST_GXT_970_SPEED_MIN;
    Breathing.brightness_min            = TRUST_GXT_970_BRIGHTNESS_MIN;
    Breathing.brightness_max            = TRUST_GXT_970_BRIGHTNESS_MAX;
    Breathing.brightness                = TRUST_GXT_970_BRIGHTNESS_MAX;
    modes.push_back(Breathing);

    mode Off;
    Off.name                            = "Off";
    Off.value                           = TRUST_GXT_970_OFF_MODE_VALUE;
    Off.flags                           = MODE_FLAG_AUTOMATIC_SAVE;
    Off.color_mode                      = MODE_COLORS_NONE;
    modes.push_back(Off);

    SetupZones();
}

RGBController_TrustGXT970::~RGBController_TrustGXT970()
{
    delete controller;
}

void RGBController_TrustGXT970::SetupZones()
{
    zone new_zone;

    new_zone.name       = "Mouse";
    new_zone.type       = ZONE_TYPE_LINEAR;
    new_zone.leds_min   = TRUST_GXT_970_NUMBER_OF_LEDS;
    new_zone.leds_max   = TRUST_GXT_970_NUMBER_OF_LEDS;
    new_zone.leds_count = TRUST_GXT_970_NUMBER_OF_LEDS;
    new_zone.matrix_map = nullptr;

    zones.emplace_back(new_zone);

    leds.resize(new_zone.leds_count);

    for(unsigned int i = 0; i < TRUST_GXT_970_NUMBER_OF_LEDS; i++)
    {
        leds[i].name = "LED " + std::to_string(i + 1);
    }

    SetupColors();
}

void RGBController_TrustGXT970::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_TrustGXT970::DeviceUpdateLEDs()
{
    DeviceUpdateMode();
}

void RGBController_TrustGXT970::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateMode();
}

void RGBController_TrustGXT970::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateMode();
}

void RGBController_TrustGXT970::DeviceUpdateMode()
{
    controller->SetMode(colors[0], modes[active_mode].brightness, modes[active_mode].speed, modes[active_mode].value);
}
