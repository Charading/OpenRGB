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

    name                = "AMD RX 6xxx GPU";
    vendor              = "Cooler Master";
    type                = DEVICE_TYPE_GPU;
    description         = cmr6000->GetDeviceName();
    version             = "1.0";
    serial              = cmr6000->GetSerial();
    location            = cmr6000->GetLocation();

    mode Off;
    Off.name            = "Turn Off";
    Off.flags           = 0;
    Off.value           = CM_MR6000_MODE_OFF;
    Off.color_mode      = MODE_COLORS_NONE;
    modes.push_back(Off);

    mode Static;
    Static.name         = "Static";
    Static.value        = CM_MR6000_MODE_STATIC;
    Static.flags        = MODE_FLAG_HAS_PER_LED_COLOR;
    Static.color_mode   = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode ColorCycle;
    ColorCycle.name       = "Color Cycle";
    ColorCycle.value      = CM_MR6000_MODE_COLOR_CYCLE;
    ColorCycle.flags      = MODE_FLAG_HAS_SPEED;
    ColorCycle.speed_min  = MR6000_CYCLE_SPEED_SLOWEST;
    ColorCycle.speed      = MR6000_CYCLE_SPEED_NORMAL;
    ColorCycle.speed_max  = MR6000_CYCLE_SPEED_FASTEST;
    ColorCycle.color_mode = MODE_COLORS_NONE;
    ColorCycle.speed      = speed;
    modes.push_back(ColorCycle);

    mode Breathing;
    Breathing.name       = "Breathing";
    Breathing.value      = CM_MR6000_MODE_BREATHE;
    Breathing.flags      = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_RANDOM_COLOR;
    Breathing.speed_min  = MR6000_BREATHE_SPEED_SLOWEST;
    Breathing.speed      = MR6000_BREATHE_SPEED_NORMAL;
    Breathing.speed_max  = MR6000_BREATHE_SPEED_FASTEST;
    Breathing.color_mode = MODE_COLORS_PER_LED;
    Breathing.speed      = speed;
    modes.push_back(Breathing);    

    SetupZones();
    active_mode = cmr6000->GetMode();
}

RGBController_CMR6000Controller::~RGBController_CMR6000Controller()
{
    delete cmr6000;
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
    unsigned char rnd = 0x00;

    if(modes[active_mode].color_mode == MODE_COLORS_RANDOM)
    {
        rnd = 0xA0;
    }
    else {
        rnd = 0x20;
    }

    cmr6000->SetMode(modes[active_mode].value, modes[active_mode].speed, red, grn, blu, rnd);
}

void RGBController_CMR6000Controller::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CMR6000Controller::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_CMR6000Controller::SetCustomMode()
{
    
}

void RGBController_CMR6000Controller::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
