#include "uDMXController.h"
#include "RGBController.h"
#include "RGBController_uDMX.h"
#include "Detector.h"
#include <libusb-1.0/libusb.h>

#define UDMX_VID 0x16C0
#define UDMX_PID 0x05DC

void DetectUDMXControllers()
{
    /*----------------------------------------------------------------------------------*\
    | Important note!!! : uDMX does not have USB serial number, so according to the USB  |
    | specification is not possible to have a duplicated device attached on the same bus |
    | with the same VID, PID and serial number. I think...                               |
    | So this controller is designed for one uDMX device attached at a time because is   |
    | tecnically imposible have multiple uDMX devices. (Unless the devices exist in      |
    | different USB busses, but the controller is not designed for that yet).            |
    | Sorry for my bad english                                                           |
    \*----------------------------------------------------------------------------------*/

    libusb_context * ctx;
    libusb_init(&ctx);

    libusb_device_handle * dev = libusb_open_device_with_vid_pid(ctx, UDMX_VID, UDMX_PID);

    if(dev)
    {

        libusb_detach_kernel_driver(dev, 1);
        libusb_claim_interface(dev, 1);

        /*-------------------------------------------*\
        | "05DC" is a shared PID with other products. |
        | Test the product string in order to detect  |
        | the device properly                         |
        \*-------------------------------------------*/

        char product_string[128];

        libusb_get_string_descriptor_ascii(dev, 3, (unsigned char *)product_string, 128);

        if(std::string(product_string) == "ilLUTZminator001")
        {
            uDMXController* controller = new uDMXController(dev);
            RGBController_uDMX* rgb_controller = new RGBController_uDMX(controller);
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        }
    }
}

 REGISTER_DETECTOR("uDMX", DetectUDMXControllers);
