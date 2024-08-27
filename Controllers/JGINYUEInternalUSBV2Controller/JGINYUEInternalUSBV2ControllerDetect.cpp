/*---------------------------------------------------------*\
| JGINYUEInternalUSBV2ControllerDetect.cpp                  |
|                                                           |
|   Detector for JGINYUE USB motherboard V2                 |
|                                                           |
|   Tong R (tcr020)                             06 Aug 2024 |
|   Liu ShiMeng(Moon dream stars)               09 Aug 2023 |
|   Dongguan Yonghang Electronic Technology Co., Ltd        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <hidapi.h>
#include <serial_port/serial_port.h>
#include <serial_port/find_usb_serial_port.h>
#include "RGBController_JGINYUEInternalUSBV2.h"
#include "JGINYUEInternalUSBV2Controller.h"
#include "RGBController.h"
#include "Detector.h"

/*---------------------------------------------------------*\
| JGINYUE vendor ID                                         |
\*---------------------------------------------------------*/
#define JGINYUE_VID_V2                                 0x1A86

/*---------------------------------------------------------*\
| JGINYUE product ID                                        |
\*---------------------------------------------------------*/
#define JGINYUE_MOTHERBOARD_PID_V2                     0xE30B

void DetectJGINYUEInternalUSBV2Controller(hid_device_info* info,const std::string& /*name*/)
{
    hid_device* hid_dev = hid_open_path(info->path);
    if(hid_dev == nullptr )
    {
        return;
    }
    
    if(hid_dev != nullptr )
    {
        std::vector<std::string*> serial_ports = find_usb_serial_port(JGINYUE_VID_V2, JGINYUE_MOTHERBOARD_PID_V2);
        if (serial_ports.size() ==1)
        {
            serial_port *port = new serial_port();
            if (!port->serial_open(serial_ports[0]->c_str(), 115200))
            {
                delete port;
                hid_close(hid_dev);
                return;
            }
            JGINYUEInternalUSBV2Controller *controller = new JGINYUEInternalUSBV2Controller(hid_dev, info->path,port);
            RGBController_JGINYUEInternalUSBV2 *rgb_controller = new RGBController_JGINYUEInternalUSBV2(controller);
            ResourceManager::get()->RegisterRGBController(rgb_controller);
        }        
    }
}

#ifdef _WIN32
REGISTER_HID_DETECTOR("JGINYUE Internal USB ControllerV2", DetectJGINYUEInternalUSBV2Controller, JGINYUE_VID_V2, JGINYUE_MOTHERBOARD_PID_V2);
#else
REGISTER_HID_DETECTOR_IPU("JGINYUE Internal USB ControllerV2", DetectJGINYUEInternalUSBV2Controller, JGINYUE_VID_V2, JGINYUE_MOTHERBOARD_PID_V2, 0, 0xFF00, 1);
#endif
