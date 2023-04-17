/*---------------------------------------------------------*\
|  BlinkStickDetector.cpp                                   |
|                                                           |
|      Detector Interface for BlinkStick LED controller     |
|                                                           |
|  Foxdogface(https://gitlab.com/Foxdogface), 29/11/2022    |
\*---------------------------------------------------------*/

#include <hidapi/hidapi.h>
#include "Detector.h"
#include "RGBController.h"

/*-----------------------------------------------------*\
| BlinkSticks specific include                          |
\*-----------------------------------------------------*/
#include "RGBController_BlinkStick.h"

/*-----------------------------------------------------*\
| Flirc USB vendor ID                                   |
\*-----------------------------------------------------*/
#define BLINK_STICK_VID  0x20A0

void DetectBlinkStick(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        BlinkStickController* controller           = new BlinkStickController(dev, info->path, info->product_id);
        RGBController_BlinkStick* rgb_controller   = new RGBController_BlinkStick(controller);


        rgb_controller->name    = controller->GetName();
        rgb_controller->serial  = controller->GetSerial();
        rgb_controller->version = rgb_controller->serial.substr(9, 3);

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR("BlinkStic", DetectBlinkStick, BLINK_STICK_VID, 0x41E5);
