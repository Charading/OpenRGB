#include "Detector.h"
#include "TesoroGramXSController.h"
#include "RGBController.h"
#include "RGBController_TesoroGramXS.h"

#include <vector>
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| Tesoro keyboard vendor and product IDs                |
\*-----------------------------------------------------*/
#define TESORO_VID              0x195D
#define GRAM_XS_PID             0x2058

void DetectTesoroGramXS(hid_device_info *info, const std::string &name)
{
    hid_device *dev = hid_open_path(info->path);

    if(dev)
    {
        TesoroGramXSController *controller          = new TesoroGramXSController(dev, info->path);
        RGBController_TesoroGramXS *rgb_controller  = new RGBController_TesoroGramXS(controller);
        rgb_controller->name                        = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_IPU("Tesoro Gram XS", DetectTesoroGramXS, TESORO_VID, GRAM_XS_PID, 2, 0xFF01, 0x0001);
