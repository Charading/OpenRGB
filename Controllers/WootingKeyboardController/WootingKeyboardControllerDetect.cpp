#include "Detector.h"
#include "WootingKeyboardController.h"
#include "RGBController.h"
#include "RGBController_WootingKeyboard.h"
#include "LogManager.h"
#include <vector>
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| Wooting vendor ID                                     |
\*-----------------------------------------------------*/
#define WOOTING_VID                         0x03EB
#define WOOTING_VID2                        0x31E3

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define WOOTING_ONE_PID                     0xFF01
#define WOOTING_TWO_PID                     0xFF02
#define WOOTING_TWO_LE_PID                  0x1210
#define WOOTING_TWO_HE_PID                  0x1220

void DetectWootingKeyboardV2(hid_device_info* info, const std::string& name)
{
    LOG_DEBUG("[Wooting KB V2] Interface %i\tPage %04X\tUsage %i\tPath %s", info->interface_number, info->usage_page, info->usage, info->path);
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        uint8_t wooting_type = (info->product_id == WOOTING_ONE_PID) ? WOOTING_KB_TKL : WOOTING_KB_FULL;
        LOG_DEBUG("[Wooting KB V2] Device type %i opened - creating controller", wooting_type);
        WootingKeyboardController* controller = new WootingKeyboardController(dev, info->path, wooting_type);

        LOG_DEBUG("[Wooting KB V2] Controller created - setting up RGB api");
        RGBController_WootingKeyboard* rgb_controller = new RGBController_WootingKeyboard(controller);
        rgb_controller->name = name;

        LOG_DEBUG("[Wooting KB V2] API intialised - Registering controller name\t%s", name);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}  /* DetectWootingKeyboardV2 */

REGISTER_HID_DETECTOR_PU("Wooting ONE Keyboard",     DetectWootingKeyboardV2,     WOOTING_VID,      WOOTING_ONE_PID,        0x1337, 1);
REGISTER_HID_DETECTOR_PU("Wooting TWO Keyboard",     DetectWootingKeyboardV2,     WOOTING_VID,      WOOTING_TWO_PID,        0x1337, 1);
REGISTER_HID_DETECTOR_PU("Wooting TWO Keyboard LE",  DetectWootingKeyboardV2,     WOOTING_VID2,     WOOTING_TWO_LE_PID,     0x1337, 1);
REGISTER_HID_DETECTOR_PU("Wooting TWO Keyboard HE",  DetectWootingKeyboardV2,     WOOTING_VID2,     WOOTING_TWO_HE_PID,     0x1337, 1);
