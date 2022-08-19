/*-------------------------------------------------------------------*\
|  RGBController_QMKXAP.cpp                                           |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#include "RGBController_QMKXAP.h"

RGBController_QMKXAP::RGBController_QMKXAP(QMKXAPController* controller_ptr)
{
    controller = controller_ptr;

    name = controller->GetName();
    vendor = controller->GetManufacturer();
    type = DEVICE_TYPE_KEYBOARD;
    description = "QMK Keyboard with XAP";
    location = controller->GetLocation();
    version = controller->GetVersion();
    serial = controller->GetHWID();
}

void RGBController_QMKXAP::SetupZones()
{

}

void RGBController_QMKXAP::ResizeZone(int /*zone*/, int /*new_size*/)
{

}

void RGBController_QMKXAP::DeviceUpdateLEDs()
{

}

void RGBController_QMKXAP::UpdateZoneLEDs(int /*zone*/)
{

}

void RGBController_QMKXAP::UpdateSingleLED(int /*led*/)
{

}

void RGBController_QMKXAP::SetCustomMode()
{

}

void RGBController_QMKXAP::DeviceUpdateMode()
{

}
