#include "Detector.h"
#include "CorsairVoidProController.h"
#include "RGBController.h"
#include "RGBController_CorsairVoidPro.h"

#include <hidapi/hidapi.h>
#include <unordered_set>

#include "LogManager.h"

/*---------------------------------------------------------*\
| Corsair vendor ID                                         |
\*---------------------------------------------------------*/
#define CORSAIR_VID                                    0x1B1C

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define CORSAIR_VOID_PRO_PID                           0x0A14

void DetectCorsairVoidProControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        CorsairVoidProController*     controller      = new CorsairVoidProController(dev, *info);
        RGBController_CorsairVoidPro* rgb_controller  = new RGBController_CorsairVoidPro(controller);
        rgb_controller->name                          = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_IPU("Corsair Void Pro", DetectCorsairVoidProControllers, CORSAIR_VID, CORSAIR_VOID_PRO_PID, 3, 0xFFC5, 0x01);
