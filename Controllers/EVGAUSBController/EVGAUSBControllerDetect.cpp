/*---------------------------------------------------------*\
| EVGAUSBControllerDetect.cpp                               |
|                                                           |
|   Detector for EVGA USB devices                           |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "HidDetector.h"
#include "RGBController_EVGAKeyboard.h"
#include "RGBController_EVGAMouse.h"

/*-----------------------------------------------------*\
| EVGA USB vendor ID                                    |
\*-----------------------------------------------------*/
#define EVGA_USB_VID                            0x3842

/*-----------------------------------------------------*\
| Keyboard product IDs                                  |
\*-----------------------------------------------------*/
#define Z15_ISO_PID                             0x260E
#define Z15_ANSI_PID                            0x2608
#define Z20_ANSI_PID                            0x260A
#define Z20_UK_PID                              0x2610

/*-----------------------------------------------------*\
| Mouse product IDs                                     |
\*-----------------------------------------------------*/
#define X20_WIRED_PID              0x2420
#define X20_WIRELESS_ADAPTER_PID   0x2402

static Controllers DetectEVGAKeyboardControllers(hid_device_info* info, const std::string& /*name*/)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if(dev)
    {
        EVGAKeyboardController*     controller      = new EVGAKeyboardController(dev, info->path, info->product_id);
        RGBController_EVGAKeyboard* rgb_controller  = new RGBController_EVGAKeyboard(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectEVGAMouse(hid_device_info* info, const std::string &, int connection_type)
{
    Controllers result;
    hid_device* dev = hid_open_path(info->path);
    if (dev)
    {
        EVGAMouseController*     controller     = new EVGAMouseController(dev, info->path, connection_type);
        RGBController_EVGAMouse* rgb_controller = new RGBController_EVGAMouse(controller);
        result.push_back(rgb_controller);
    }
    return result;
}

static Controllers DetectWiredEVGAMouse(hid_device_info* info, const std::string &name)
{
    return DetectEVGAMouse(info, name, EVGA_PERIPHERAL_CONNECTION_TYPE_WIRED);
}

static Controllers DetectWirelessEVGAMouse(hid_device_info* info, const std::string &name)
{
    return DetectEVGAMouse(info, name, EVGA_PERIPHERAL_CONNECTION_TYPE_WIRELESS);
}


REGISTER_HID_DETECTOR_IPU("EVGA Z15 Keyboard",     DetectEVGAKeyboardControllers,   EVGA_USB_VID,   Z15_ISO_PID,                1,  0x08,   0x4B);
REGISTER_HID_DETECTOR_IPU("EVGA Z15 Keyboard",     DetectEVGAKeyboardControllers,   EVGA_USB_VID,   Z15_ANSI_PID,               1,  0x08,   0x4B);
REGISTER_HID_DETECTOR_IPU("EVGA Z20 Keyboard",     DetectEVGAKeyboardControllers,   EVGA_USB_VID,   Z20_ANSI_PID,               1,  0x08,   0x4B);
REGISTER_HID_DETECTOR_IPU("EVGA Z20 Keyboard",     DetectEVGAKeyboardControllers,   EVGA_USB_VID,   Z20_UK_PID,                 1,  0x08,   0x4B);

REGISTER_HID_DETECTOR_IPU("EVGA X20 Gaming Mouse",  DetectWiredEVGAMouse,           EVGA_USB_VID,   X20_WIRED_PID,              2,  0xFFFF, 0);
REGISTER_HID_DETECTOR_IPU("EVGA X20 USB Receiver",  DetectWirelessEVGAMouse,        EVGA_USB_VID,   X20_WIRELESS_ADAPTER_PID,   2,  0xFFFF, 0);
