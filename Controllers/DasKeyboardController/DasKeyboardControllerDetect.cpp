#include "Detector.h"
#include "DasKeyboardController.h"
#include "RGBController.h"
#include "RGBController_DasKeyboard.h"
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| Das vendor ID                                         |
\*-----------------------------------------------------*/
#define DAS_VID                         0x24F0

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define DAS_Q4_PID                      0x2037
#define DAS_Q5_PID                      0x202B

/******************************************************************************************\
*                                                                                          *
*   DetectDasKeyboardControllers                                                           *
*                                                                                          *
*       Tests the USB address to see if a Das RGB Keyboard controller exists there.        *
*       We need the second interface to communicate with the keyboard                      *
*                                                                                          *
\******************************************************************************************/

void DetectDasKeyboardControllers(hid_device_info *info_in, const std::string &name) {
    hid_device_info *info = info_in;
    while (info) {
        if (info->vendor_id == DAS_VID &&
            info->product_id == DAS_Q4_PID &&
            info->interface_number == 1) {
            break;
        }
        info = info->next;
    }
    if (!info) {
        return;
    }

    hid_device *dev = hid_open_path(info->path);
    if (dev) {
        auto *controller = new DasKeyboardController(dev, info->path);
        auto *rgb_controller = new RGBController_DasKeyboard(controller);
        rgb_controller->SetupZones();
        rgb_controller->name = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}   /* DetectDasKeyboardControllers() */

REGISTER_HID_DETECTOR_I("Das Q4 RGB", DetectDasKeyboardControllers, DAS_VID, DAS_Q4_PID, 0); // NOLINT(cert-err58-cpp)
REGISTER_HID_DETECTOR_I("Das Q5 RGB", DetectDasKeyboardControllers, DAS_VID, DAS_Q5_PID, 0); // NOLINT(cert-err58-cpp)
