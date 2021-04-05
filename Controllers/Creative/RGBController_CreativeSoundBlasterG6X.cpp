#include "RGBController_CreativeSoundBlasterG6X.h"

RGBController_CreativeSoundBlasterG6X::RGBController_CreativeSoundBlasterG6X(CreativeSoundBlasterG6XController* creative_device)
{
    creative = creative_device;

    name        = "Creative SoundBlasterX G6";
    vendor      = "Creative";
    type        = DEVICE_TYPE_UNKNOWN;
    description = "Creative External USB sound card";
    location    = creative_device->GetDeviceLocation();
    serial      = "";

    mode Static;
    Static.name                     = "Static";
    Static.value                    = 0;
    Static.flags                    = MODE_COLORS_PER_LED;
    Static.color_mode               = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    SetupZones();
}

RGBController_CreativeSoundBlasterG6X::~RGBController_CreativeSoundBlasterG6X()
{
    delete creative;
}

void RGBController_CreativeSoundBlasterG6X::SetupZones()
{
    zone logo_zone;
    logo_zone.name                  = "Logo";
    logo_zone.type                  = ZONE_TYPE_SINGLE;
    logo_zone.leds_min              = 1;
    logo_zone.leds_max              = 1;
    logo_zone.leds_count            = 1;
    logo_zone.matrix_map            = NULL;
    zones.push_back(logo_zone);

    led logo_led;
    logo_led.name = "Logo";
    leds.push_back(logo_led);

    SetupColors();
}

void RGBController_CreativeSoundBlasterG6X::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CreativeSoundBlasterG6X::DeviceUpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);

    creative->SetLedColor(red, grn, blu);
}

void RGBController_CreativeSoundBlasterG6X::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CreativeSoundBlasterG6X::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CreativeSoundBlasterG6X::SetCustomMode()
{
    DeviceUpdateLEDs();
}

void RGBController_CreativeSoundBlasterG6X::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
