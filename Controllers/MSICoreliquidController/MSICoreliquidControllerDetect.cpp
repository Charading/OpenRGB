#include <hidapi/hidapi.h>
#include "Detector.h"
#include "RGBController.h"

#include "MSICoreliquidController.h"
#include "RGBController_MSICoreliquidK360.h"

/*--------------------------------*\
| USB identification definitions   |
\*_-------------------------------*/
#define CORELIQUID_VID            0x0DB0
#define CORELIQUID_K360V2_PID     0xB130

void DetectCoreliquidK360V2(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        MSICoreliquidController* controller             = new MSICoreliquidController(dev, *info);
        RGBController_MSICoreliquidK360* rgb_controller = new RGBController_MSICoreliquidK360(controller);
        rgb_controller->name                            = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_IPU("MSI MPG CORELIQUID K360V2", DetectCoreliquidK360V2, CORELIQUID_VID, CORELIQUID_K360V2_PID, 0, 0xFF00, 1);
