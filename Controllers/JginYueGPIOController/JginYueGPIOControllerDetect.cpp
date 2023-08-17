#include "RGBController_JginYueGPIO.h"
#include "JginYueGPIOController.h"
#include "RGBController.h"
#include "Detector.h"
#include "i2c_smbus.h"
#include "pci_ids.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>

/*---------------------------------------------------------*\
| JGINYUE vendor ID                                         |
\*---------------------------------------------------------*/
#define JGINYUE_VID                                 0x0416

/*---------------------------------------------------------*\
| JGINYUE product ID                                         |
\*---------------------------------------------------------*/
#define JGINYUE_MOTHERBOARD_PID                     0xA125

void DetectJginYueGPIOController(hid_device_info* info,const std::string& /*name*/)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        JginYueGPIOController*       controller      =new JginYueGPIOController(dev,info->path);
        RGBController_JginYueGPIO*   rgb_controller  =new RGBController_JginYueGPIO(controller);
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR("JginYue Internal USB Controller",DetectJginYueGPIOController,JGINYUE_VID,JGINYUE_MOTHERBOARD_PID);
