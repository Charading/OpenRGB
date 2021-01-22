#include "Detector.h"
#include "DASKeyboardController.h"
#include "RGBController.h"
#include "RGBController_DASKeyboard.h"
#include <hidapi/hidapi.h>

/*-----------------------------------------------------*\
| DAS vendor ID                                         |
\*-----------------------------------------------------*/
#define DAS_VID                         0x24F0

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define DAS_Q4_PID                      0x2037
#define DAS_Q5_PID                      0x202B

/******************************************************************************************\
*                                                                                          *
*   DetectDASKeyboardControllers                                                           *
*                                                                                          *
*       Tests the USB address to see if a DAS RGB Keyboard controller exists there.        *
*       We need the second interface to communicate with the keyboard                      *
*                                                                                          *
\******************************************************************************************/

void DetectDASKeyboardControllers(hid_device_info *info_in, const std::string &name) {
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
        auto *controller = new DASKeyboardController(dev, info->path);
        auto *rgb_controller = new RGBController_DASKeyboard(controller);
        rgb_controller->SetupZones();
        rgb_controller->name = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}   /* DetectDASKeyboardControllers() */

REGISTER_HID_DETECTOR_I("DAS Q4 RGB", DetectDASKeyboardControllers, DAS_VID, DAS_Q4_PID, 0); // NOLINT(cert-err58-cpp)
REGISTER_HID_DETECTOR_I("DAS Q5 RGB", DetectDASKeyboardControllers, DAS_VID, DAS_Q5_PID, 0); // NOLINT(cert-err58-cpp)
