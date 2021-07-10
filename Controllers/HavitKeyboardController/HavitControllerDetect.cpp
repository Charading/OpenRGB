#include <hidapi/hidapi.h>
#include "Detector.h"
#include "RGBController.h"

#include "HavitKB395LController.h"
#include "RGBController_HavitKB395L.h"


/*-----------------------------------------------------*\
| Keyboard USB Values                                   |
\*-----------------------------------------------------*/

#define HAVIT_KEYBOARD_VID          0x04D9 
#define HAVIT_KB395L_PID            0xA1DD

/******************************************************************************************\
*                                                                                          *
*   DetectHavitKeyboard                                                                    *
*                                                                                          *
*       Tests the USB address to see if a Havit Keyboard Controller exist there.           *
*                                                                                          *
\******************************************************************************************/

void DetectHavitController(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);
    if( dev )
    {
        HavitKB395LController* controller = new HavitKB395LController(dev, info->path);
        RGBController_HavitKB395L* rgb_controller = new RGBController_HavitKB395L(controller);
        rgb_controller->name = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }

}   /* DetectHavitController() */

REGISTER_HID_DETECTOR_IPU("Havit Keyboard HV-KB395L", DetectHavitController, HAVIT_KEYBOARD_VID, HAVIT_KB395L_PID, 0, 0xFF01, 1);
