#include "Detector.h"
#include "MSI3ZoneController.h"
#include "RGBController.h"
#include "RGBController_MSI3Zone.h"
#include <hidapi/hidapi.h>

#define MSI_3_ZONE_KEYBOARD_VID 0x1770
#define MSI_3_ZONE_KEYBOARD_PID 0xFF00

/******************************************************************************************\
*                                                                                          *
*   DetectMSI3ZoneControllers                                                              *
*                                                                                          *
*       Tests the USB address to see if an MSI/SteelSeries 3-zone Keyboard controller      *
*       exists there.                                                                      *
*                                                                                          *
\******************************************************************************************/

void DetectMSI3ZoneControllers(hid_device_info* info, const std::string&)
{
    hid_device* dev = hid_open_path(info->path);
    if( dev )
    {
        MSI3ZoneController* controller = new MSI3ZoneController(dev, info->path);
        RGBController_MSI3Zone* rgb_controller = new RGBController_MSI3Zone(controller);
        // Constructor sets the name
        ResourceManager::get()->RegisterRGBController(rgb_controller, CONTROLLER_LIST_HID);
    }
}   /* DetectMSI3ZoneControllers() */

REGISTER_HID_DETECTOR("MSI 3-Zone Laptop", DetectMSI3ZoneControllers, MSI_3_ZONE_KEYBOARD_VID, MSI_3_ZONE_KEYBOARD_PID);
