#include "Detector.h"
#include "CorsairEliteCapellixController.h"
#include "RGBController.h"
#include "RGBController_CorsairEliteCapellix.h"
#include <vector>
#include <hidapi/hidapi.h>
#include <iostream>
using namespace std;

/*-----------------------------------------------------*\
| Corsair vendor ID                                     |
\*-----------------------------------------------------*/
#define CORSAIR_VID                     0x1B1C

/*-----------------------------------------------------*\
| Elite Capellix product IDs                            |
\*-----------------------------------------------------*/
#define CORSAIR_H150I_ELITE_CAPELLIX_PID       0x0C1C

typedef struct
{
    unsigned short  usb_vid;
    unsigned short  usb_pid;
    unsigned char   usb_interface;
    const char *    name;
} corsair_Capellix_device;

#define CORSAIR_NUM_DEVICES (sizeof(device_list) / sizeof(device_list[ 0 ]))

/******************************************************************************************\
*                                                                                          *
*   DetectCorsairCapellixControllers                                                       *
*                                                                                          *
*       Tests the USB address to see if a Corsair RGB Cooler controller exists there.      *
*                                                                                          *
\******************************************************************************************/

void DetectCorsairCapellixHIDControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if( dev )
    {
        CorsairEliteCapellixController* controller = new CorsairEliteCapellixController(dev, info->path);
        RGBController_CorsairEliteCapellix* rgb_controller = new RGBController_CorsairEliteCapellix(controller);
        rgb_controller->name = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_PU("Corsair H150i Elite Capellix",       DetectCorsairCapellixHIDControllers, CORSAIR_VID, CORSAIR_H150I_ELITE_CAPELLIX_PID, 0xFF42, 0x01);
