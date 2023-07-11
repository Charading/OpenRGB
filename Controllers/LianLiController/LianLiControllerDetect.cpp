/*---------------------------------------------------------*\
| LianLiControllerDetect.cpp                                |
|                                                           |
|   Detector for Lian Li devices                            |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <libusb.h>
#include "Detector.h"
#include "HidDetector.h"
#include "ResourceManager.h"

/*-----------------------------------------------------*\
| LianLi USB Controller specific includes               |
\*-----------------------------------------------------*/
#include "RGBController_LianLiUniHub.h"
#include "RGBController_LianLiStrimerLConnect.h"
#include "LianLiUniHubController.h"
#include "RGBController_LianLiUniHub.h"
#include "LianLiUniHubALController.h"
#include "RGBController_LianLiUniHubAL.h"
#include "LianLiUniHub_AL10Controller.h"
#include "RGBController_LianLiUniHub_AL10.h"
#include "LianLiUniHubSLV2Controller.h"
#include "RGBController_LianLiUniHubSLV2.h"
#include "LianLiUniHubSLInfinityController.h"
#include "RGBController_LianLiUniHubSLInfinity.h"
#include "LianLiGAIITrinityController.h"
#include "RGBController_LianLiGAIITrinity.h"

/*-----------------------------------------------------*\
| USB vendor IDs                                        |
\*-----------------------------------------------------*/
#define ENE_USB_VID                                 0x0CF2
#define NUVOTON_USB_VID                             0x0416

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define STRIMER_L_CONNECT_PID                       0xA200

/*-----------------------------------------------------*\
| Fan controller product IDs                            |
\*-----------------------------------------------------*/
#define UNI_HUB_PID                                 0x7750
#define UNI_HUB_AL_PID                              0xA101
#define UNI_HUB_SLINF_PID                           0xA102
#define UNI_HUB_SLV2_PID                            0xA103
#define UNI_HUB_ALV2_PID                            0xA104
#define UNI_HUB_SLV2_V05_PID                        0xA105
#define GAII_USB_PID                                0x7373
#define GAII_Perf_USB_PID                           0x7371

/*----------------------------------------------------------------------------*\
| The Uni Hub is controlled by sending control transfers to various wIndex     |
| addresses, allthough it announces some kind of hid interface. Hence it       |
| requires libusb as hidapi provides no wIndex customization.                  |
\*----------------------------------------------------------------------------*/

void DetectLianLiUniHub()
{
    libusb_device** devices = nullptr;

    ssize_t ret;

    ret = libusb_init(NULL);
    if(ret < 0)
    {
        return;
    }

    ret = libusb_get_device_list(NULL, &devices);
    if(ret < 0)
    {
        return;
    }

    ssize_t deviceCount = ret;

    for(int i = 0; i < deviceCount; i++)
    {
        libusb_device* device = devices[i];
        libusb_device_descriptor descriptor;
        ret = libusb_get_device_descriptor(device, &descriptor);

        if(ret < 0)
        {
            continue;
        }

        if( descriptor.idVendor  == ENE_USB_VID
         && descriptor.idProduct == UNI_HUB_PID)
        {
            LianLiUniHubController*     controller     = new LianLiUniHubController(device, &descriptor);
            RGBController_LianLiUniHub* rgb_controller = new RGBController_LianLiUniHub(controller);
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        }
    }

    if(devices != nullptr)
    {
        libusb_free_device_list(devices, 1);
    }
}

static Controllers DetectLianLiUniHub_AL10()
{
    Controllers result;
    libusb_device** devices = nullptr;

    ssize_t ret;

    ret = libusb_init(NULL);
    if(ret < 0)
    {
        return result;
    }

    ret = libusb_get_device_list(NULL, &devices);
    if(ret < 0)
    {
        return result;
    }

    ssize_t deviceCount = ret;

    for(int i = 0; i < deviceCount; i++)
    {
        libusb_device* device = devices[i];
        libusb_device_descriptor descriptor;
        ret = libusb_get_device_descriptor(device, &descriptor);

        if(ret < 0)
        {
            continue;
        }

        if( descriptor.idVendor  == ENE_USB_VID
         && descriptor.idProduct == UNI_HUB_AL_PID)
        {
            LianLiUniHub_AL10Controller*     controller     = new LianLiUniHub_AL10Controller(device, &descriptor);
            RGBController_LianLiUniHub_AL10* rgb_controller = new RGBController_LianLiUniHub_AL10(controller);
            result.push_back(rgb_controller);
        }
    }

    if(devices != nullptr)
    {
        libusb_free_device_list(devices, 1);
    }
    return result;
}   /* DetectLianLiUniHub_AL10() */

static Controllers DetectLianLiUniHubAL(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LianLiUniHubALController* controller = new LianLiUniHubALController(dev, info->path, info->product_id, name);

        std::string firmwareVersion = controller->GetFirmwareVersionString();

        if(firmwareVersion == "v1.7")
        {
            RGBController_LianLiUniHubAL* rgb_controller = new RGBController_LianLiUniHubAL(controller);
            result.push_back(rgb_controller);
        }
        else if(firmwareVersion == "v1.0")
        {
            // We fall back to libusb detection
            // NOTE: Location will be overriden
            delete controller;
            result = DetectLianLiUniHub_AL10();
        }
        else
        {
            delete controller;
        }
    }
    return result;
}   /* DetectLianLiUniHubAL() */

static Controllers DetectLianLiUniHubSLV2(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LianLiUniHubSLV2Controller* controller = new LianLiUniHubSLV2Controller(dev, info->path, name);
        RGBController_LianLiUniHubSLV2* rgb_controller = new RGBController_LianLiUniHubSLV2(controller);
        result.push_back(rgb_controller);
    }
    return result;
}   /* DetectLianLiUniHubSLV2() */

static Controllers DetectLianLiUniHubSLInfinity(hid_device_info* info, const std::string& name)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LianLiUniHubSLInfinityController* controller = new LianLiUniHubSLInfinityController(dev, info->path, name);

        RGBController_LianLiUniHubSLInfinity* rgb_controller = new RGBController_LianLiUniHubSLInfinity(controller);
        result.push_back(rgb_controller);
    }
    return result;
}   /* DetectLianLiUniHubSLInfinity() */

GENERIC_HOTPLUGGABLE_DETECTOR(DetectLianLiStrimerControllers, LianLiStrimerLConnectController, RGBController_LianLiStrimerLConnect)

static Controllers DetectLianLiGAIITrinity(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        LianLiGAIITrinityController*     controller     = new LianLiGAIITrinityController(dev);
        RGBController_LianLiGAIITrinity* rgb_controller = new RGBController_LianLiGAIITrinity(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

REGISTER_DETECTOR("Lian Li Uni Hub",                            DetectLianLiUniHub);
REGISTER_HID_DETECTOR_IPU("Lian Li Uni Hub - AL",               DetectLianLiUniHubAL,           ENE_USB_VID,        UNI_HUB_AL_PID,           0x01,   0xFF72, 0xA1);
REGISTER_HID_DETECTOR_IPU("Lian Li Uni Hub - SL V2",            DetectLianLiUniHubSLV2,         ENE_USB_VID,        UNI_HUB_SLV2_PID,         0x01,   0xFF72, 0xA1);
REGISTER_HID_DETECTOR_IPU("Lian Li Uni Hub - AL V2",            DetectLianLiUniHubSLV2,         ENE_USB_VID,        UNI_HUB_ALV2_PID,         0x01,   0xFF72, 0xA1);
REGISTER_HID_DETECTOR_IPU("Lian Li Uni Hub - SL V2 v0.5",       DetectLianLiUniHubSLV2,         ENE_USB_VID,        UNI_HUB_SLV2_V05_PID,     0x01,   0xFF72, 0xA1);
REGISTER_HID_DETECTOR_IPU("Lian Li Uni Hub - SL Infinity",      DetectLianLiUniHubSLInfinity,   ENE_USB_VID,        UNI_HUB_SLINF_PID,        0x01,   0xFF72, 0xA1);
/*---------------------------------------------------------------------------------------------------------*\
| Entries for dynamic UDEV rules                                                                            |
|                                                                                                           |
| DUMMY_DEVICE_DETECTOR("Lian Li Uni Hub", DetectLianLiUniHub, 0x0CF2, 0x7750 )                             |
\*---------------------------------------------------------------------------------------------------------*/

REGISTER_HID_DETECTOR_IPU("Strimer L Connect",                  DetectLianLiStrimerControllers, ENE_USB_VID,        STRIMER_L_CONNECT_PID,       1,   0xFF72, 0xA1);
REGISTER_HID_DETECTOR_I("Lian Li GA II Trinity",                DetectLianLiGAIITrinity,        NUVOTON_USB_VID,    GAII_USB_PID,                             0x02);
REGISTER_HID_DETECTOR_I("Lian Li GA II Trinity Performance",    DetectLianLiGAIITrinity,        NUVOTON_USB_VID,    GAII_Perf_USB_PID,                        0x02);
