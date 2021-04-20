#include "Detector.h"
#include "CorsairCapellixController.h"
#include "RGBController.h"
#include "RGBController_CorsairCapellix.h"
#include <vector>
#include <libusb-1.0/libusb.h>
#include <hidapi/hidapi.h>
#include <iostream>
using namespace std;

/*-----------------------------------------------------*\
| Corsair vendor ID                                     |
\*-----------------------------------------------------*/
#define CORSAIR_VID                     0x1B1C

/*-----------------------------------------------------*\
| Keyboard Capellix Series product IDs                  |
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

static const corsair_Capellix_device device_list[] =
{
    /*-----------------------------------------------------------------------------------------------------*\
    | Coolers                                                                                               |
    \*-----------------------------------------------------------------------------------------------------*/
    { CORSAIR_VID,          CORSAIR_H150I_ELITE_CAPELLIX_PID,          0,      "Corsair H150i Elite Capellix"             },

};

/******************************************************************************************\
*                                                                                          *
*   DetectCorsairCapellixControllers                                                       *
*                                                                                          *
*       Tests the USB address to see if a Corsair RGB Cooler controller exists there.      *
*                                                                                          *
\******************************************************************************************/

//void DetectCorsairCapellixControllers(hid_device_info* info, const std::string& name)

/*
void DetectCorsairCapellixControllers(std::vector<RGBController*>& rgb_controllers)
{

    libusb_context * ctx;
    libusb_init(&ctx);
    #ifdef _WIN32
    libusb_set_option(ctx, LIBUSB_OPTION_USE_USBDK);
    #endif
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    libusb_device_handle **devp;
    ssize_t cnt; //holding number of devices in list
    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        //return false;
        //there was an error
    }
    //print total number of usb devices
    ssize_t i; //for iterating through the list
    for(i = 0; i < cnt; i++) {
        struct libusb_device_descriptor desc;
        if (libusb_get_device_descriptor(devs[i], &desc) >= 0) {
            if (desc.idProduct == 0x0C1C){
                int a = libusb_open(devs[i], devp);
                std::cout << desc.idProduct << endl;
                std::cout << a << endl;
            }
        }
    }
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it


}  */


void DetectCorsairCapellixHIDControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if( dev )
    {
        CorsairCapellixController* controller = new CorsairCapellixController(dev, info->path);
        RGBController_CorsairCapellix* rgb_controller = new RGBController_CorsairCapellix(controller);
        rgb_controller->name = name;
        std::cout << name << endl;
        std::cout << info->interface_number << endl;
        std::cout << info->path << endl;
        std::cout << info->usage_page << endl;
        std::cout << info->usage << endl;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}




//REGISTER_DETECTOR("Corsair Capellix Series", DetectCorsairCapellixControllers);
REGISTER_HID_DETECTOR_PU("Corsair H150i Elite Capellix",       DetectCorsairCapellixHIDControllers, CORSAIR_VID, CORSAIR_H150I_ELITE_CAPELLIX_PID, 0xFF42, 0x01);
