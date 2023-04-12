/*---------------------------------------------------------*\
|  Roccat Kone Pure SEL                                     |
|                                                           |
|  Warren Dumortier (nwarrenfl@gmail.com), 23/03/2023       |
\*---------------------------------------------------------*/

#include "RGBController_RoccatKonePureSEL.h"

RGBController_RoccatKonePureSEL::RGBController_RoccatKonePureSEL(RoccatKonePureSELController* controller_ptr)
{
    controller  = controller_ptr;

    name        = "Roccat Kone Pure SEL";
    vendor      = "Roccat";
    type        = DEVICE_TYPE_MOUSE;
    description = "Roccat Kone Pure SEL Mouse";
    location    = controller->GetLocation();
    serial      = controller->GetSerial();

    mode Off;
    Off.name            = "Off";
    Off.value           = ROCCAT_KONE_PURE_SEL_MODE_OFF;
    modes.push_back(Off);

    mode Static;
    Static.name         = "Static";
    Static.value        = ROCCAT_KONE_PURE_SEL_MODE_STATIC;
    Static.flags        = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR;
    Static.color_mode   = MODE_COLORS_MODE_SPECIFIC;
    Static.colors.resize(1);
    modes.push_back(Static);

    mode Breathing;
    Breathing.name      = "Breathing";
    Breathing.value     = ROCCAT_KONE_PURE_SEL_MODE_BREATHING;
    modes.push_back(Breathing);

    active_mode = ROCCAT_KONE_PURE_SEL_MODE_OFF;
}

RGBController_RoccatKonePureSEL::~RGBController_RoccatKonePureSEL()
{
    delete controller;
}

void RGBController_RoccatKonePureSEL::DeviceUpdateLEDs()
{
    controller->SetMode(modes[active_mode].value, modes[active_mode].colors);
    controller->SendUpdate();
}

void RGBController_RoccatKonePureSEL::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}

void RGBController_RoccatKonePureSEL::UpdateZoneLEDs(int /*zone_idx*/)
{
}

void RGBController_RoccatKonePureSEL::UpdateSingleLED(int /*led_idx*/)
{
}

void RGBController_RoccatKonePureSEL::SetupZones()
{
}

void RGBController_RoccatKonePureSEL::ResizeZone(int /*zone*/, int /*new_size*/)
{
}
