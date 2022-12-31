#include "Detector.h"
#include "TrustGXT114Controller.h"
#include "TrustGXT180Controller.h"
#include "TrustGXT970Controller.h"
#include "RGBController.h"
#include "RGBController_TrustGXT114.h"
#include "RGBController_TrustGXT180.h"
#include "RGBController_TrustGXT970.h"

/*---------------------------------------------------------*\
| Trust vendor ID                                           |
\*---------------------------------------------------------*/
#define TRUST_VID                                      0x145F

/*---------------------------------------------------------*\
| Product IDs                                               |
\*---------------------------------------------------------*/
#define TRUST_GXT_114_PID                              0x026D
#define TRUST_GXT_180_PID                              0x0248
#define TRUST_GXT_970_PID                              0x02B9

void DetectTrustGXT114Controllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        TrustGXT114Controller* controller               = new TrustGXT114Controller(dev, *info);

        if(controller->Test())
        {
            RGBController_TrustGXT114* rgb_controller   = new RGBController_TrustGXT114(controller);
            rgb_controller->name = name;
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        }
        else
        {
            delete controller;
        }
    }
}

void DetectTrustGXT180Controllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        TrustGXT180Controller* controller           = new TrustGXT180Controller(dev, *info);
        RGBController_TrustGXT180* rgb_controller   = new RGBController_TrustGXT180(controller);
        rgb_controller->name                        = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

void DetectTrustGXT970Controllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        TrustGXT970Controller* controller           = new TrustGXT970Controller(dev, *info);
        RGBController_TrustGXT970* rgb_controller   = new RGBController_TrustGXT970(controller);
        rgb_controller->name                        = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_IPU("Trust GXT 114", DetectTrustGXT114Controllers, TRUST_VID, TRUST_GXT_114_PID, 1, 0xFF00, 1);
REGISTER_HID_DETECTOR_IPU("Trust GXT 180", DetectTrustGXT180Controllers, TRUST_VID, TRUST_GXT_180_PID, 1, 0xFFA0, 1);
REGISTER_HID_DETECTOR_IPU("Trust GXT 970", DetectTrustGXT970Controllers, TRUST_VID, TRUST_GXT_970_PID, 1, 0xFF00, 1);
