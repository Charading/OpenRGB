/*-------------------------------------------------------------------*\
|  RGBController_CMR6000Controller.cpp                                |
|                                                                     |
|  Driver for Coolermaster based AMD Radeon GPU (6000 series)         |
|                                                                     |
|  Eric S (edbgon)          2nd Feb 2021                              |
|                                                                     |
\*-------------------------------------------------------------------*/

#include "RGBController_CMR6000Controller.h"

RGBController_CMR6000Controller::RGBController_CMR6000Controller(CMR6000Controller* cmmp_ptr)
{
    cmr6000             = cmmp_ptr;
    unsigned char speed = cmr6000->GetLedSpeed();

    name                = cmr6000->GetDeviceName();
    vendor              = "Cooler Master";
    type                = DEVICE_TYPE_GPU;
    description         = cmr6000->GetDeviceName();
    version             = "1.0";
    serial              = cmr6000->GetSerial();
    location            = cmr6000->GetLocation();

    mode Off;
    Off.name            = "Turn Off";
    Off.value           = CM_MR6000_MODE_OFF;
    Off.color_mode      = MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name         = "Static";
    Static.value        = CM_MR6000_MODE_STATIC;
    Static.flags        = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode   = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    SetupZones();
    active_mode = cmr6000->GetMode();
}

RGBController_CMR6000Controller::~RGBController_CMR6000Controller()
{

}

void RGBController_CMR6000Controller::SetupZones()
{
    zone GP_zone;
    GP_zone.name          = "GPU";
    GP_zone.type          = ZONE_TYPE_SINGLE;
    GP_zone.leds_min      = 1;
    GP_zone.leds_max      = 1;
    GP_zone.leds_count    = 1;
    GP_zone.matrix_map    = NULL;
    zones.push_back(GP_zone);

    led GP_led;
    GP_led.name = "Logo";
    leds.push_back(GP_led);

    SetupColors();

    /*---------------------------------------------------------*\
    | Initialize colors for each LED                            |
    \*---------------------------------------------------------*/
    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        unsigned char red = cmr6000->GetLedRed();
        unsigned char grn = cmr6000->GetLedGreen();
        unsigned char blu = cmr6000->GetLedBlue();

        colors[led_idx] = ToRGBColor(red, grn, blu);
    }
}

void RGBController_CMR6000Controller::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_CMR6000Controller::DeviceUpdateLEDs()
{
    unsigned char red = RGBGetRValue(colors[0]);
    unsigned char grn = RGBGetGValue(colors[0]);
    unsigned char blu = RGBGetBValue(colors[0]);
    cmr6000->SetColor(red, grn, blu);
}

void RGBController_CMR6000Controller::UpdateZoneLEDs(int zone)
{
    RGBColor      color = colors[zone];
    unsigned char red   = RGBGetRValue(color);
    unsigned char grn   = RGBGetGValue(color);
    unsigned char blu   = RGBGetBValue(color);
    cmr6000->SetColor(red, grn, blu);
}

void RGBController_CMR6000Controller::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_CMR6000Controller::SetCustomMode()
{
    active_mode = 1;
}

void RGBController_CMR6000Controller::DeviceUpdateMode()
{
    cmr6000->SetMode(modes[active_mode].value, modes[active_mode].speed);
}
