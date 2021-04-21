#include "Detector.h"
#include "AlienwareController.h"
#include "RGBController.h"
#include "RGBController_Alienware.h"

/*---------------------------------------------------------*\
| Alienware vendor ID                                       |
\*---------------------------------------------------------*/
#define ALIENWARE_VID                               0x187c

/*---------------------------------------------------------*\
| Alienware product ID                                      |
\*---------------------------------------------------------*/
#define ALIENWARE_PID                               0x0550

void DetectAlienwareControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        auto controller = std::make_unique<AlienwareController>(dev, *info, name);
        RGBController_Alienware* rgb_controller = new RGBController_Alienware(std::move(controller));
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR("Alienware Corporation LED Controller", DetectAlienwareControllers, ALIENWARE_VID, ALIENWARE_PID);
