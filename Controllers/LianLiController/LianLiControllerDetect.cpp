/*-----------------------------------------*\
|  LianLiControllerDetect.cpp               |
|                                           |
|  Detector for Lian Li Uni Hub USB         |
|  lighting controller                      |
|                                           |
|  Luca Lovisa 2/20/2021                    |
\*-----------------------------------------*/

#include <stdexcept>
#include <string>
#include <vector>

#include "Detector.h"
#include "LianLiUniHubController.h"
#include "RGBController_LianLiUniHub.h"
#include "ResourceManager.h"

#include <libusb-1.0/libusb.h>

#define UNI_HUB_VID 0x0CF2
#define UNI_HUB_PID 0x7750

/*----------------------------------------------------------------------------*\
| The Uni Hub is controlled by sending control transfers to various wIndex     |
| addresses, allthough it announces some kind of hid interface. Hence it       |
| requires libusb as hidapi provides no wIndex customization.                  |
\*----------------------------------------------------------------------------*/

void DetectLianLiUniHub(std::vector<RGBController*>& rgb_controllers)
{
    libusb_context* context = nullptr;
    libusb_device** devices = nullptr;

    try {
        int ret;

        ret = libusb_init(&context);
        if (ret < 0)
        {
            throw std::runtime_error("USB initialization failed");
        }

        ret = libusb_get_device_list(context, &devices);
        if (ret < 0)
        {
            throw std::runtime_error("USB device listing failed");
        }

        int deviceCount = ret;

        for (int i = 0; i < deviceCount; i++)
        {
            libusb_device* device = devices[i];
            libusb_device_descriptor descriptor;
            ret = libusb_get_device_descriptor(device, &descriptor);

            if (ret < 0)
            {
                /* weird, but ok ... */
                continue;
            }

            if (       descriptor.idVendor  == UNI_HUB_VID
                    && descriptor.idProduct == UNI_HUB_PID)
            {
                LianLiUniHubController* controller = new LianLiUniHubController(device, &descriptor);
                RGBController_LianLiUniHub* rgb_controller = new RGBController_LianLiUniHub(controller);
                rgb_controllers.push_back(rgb_controller);
            }
        }
    }
    catch (...) {
        /* Something went wrong, we'll stay in the dark :( */
    }

    if (devices != nullptr) {
        libusb_free_device_list(devices, 1);
    }

    if (context != nullptr) {
        libusb_exit(context);
    }
}

// UNI_HUB_VID, UNI_HUB_PID

REGISTER_DETECTOR("Lian Li Uni Hub", DetectLianLiUniHub);
