/*---------------------------------------------------------*\
| JGINYUEInternalUSBV2Controller.cpp                          |
|                                                           |
|   Driver for JGINYUE USB motherboard                      |
|                                                           |
|   Tong R (tcr020)                             09 Aug 2023 |
|   Liu ShiMeng(Moon dream stars)               09 Aug 2023 |
|   Dongguan Yonghang Electronic Technology Co., Ltd        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <cstring>
#include <string>
#include <stdio.h>
#include "RGBController.h"
#include "ResourceManager.h"
#include "SettingsManager.h"
#include "JGINYUEInternalUSBV2Controller.h"

#include "dmiinfo.h"

#define JGINYUE_HID_GENERAL_COMMAND_HEADER              0x01
#define JGINYUE_HID_CDC_COMMAND_HEADER                  0x10
#define JGINYUE_HID_FUNCTION_GLOBAL_COMMAND             0x00



using namespace std::chrono_literals;

JGINYUEInternalUSBV2Controller::JGINYUEInternalUSBV2Controller(hid_device* jy_hid_device, const char* path,serial_port* jy_cdc_device)
{
    DMIInfo                     dmi;
    jy_hid_interface            = jy_hid_device;
    jy_cdc_interface            = jy_cdc_device;
    location                    = path;
    device_name                 = "JGINYUE " + dmi.getMainboard() + " Internal USB Controller V2";

    Init_device();
}

JGINYUEInternalUSBV2Controller::~JGINYUEInternalUSBV2Controller()
{
    hid_close(jy_hid_interface);
    delete jy_cdc_interface;

}

unsigned int JGINYUEInternalUSBV2Controller::GetZoneCount()
{
    return(JGINYUE_MAX_ZONES);
}

std::string JGINYUEInternalUSBV2Controller::GetDeviceLocation()
{
    return("HID:" + location);
}

std::string JGINYUEInternalUSBV2Controller::GetDeviceName()
{
    return(device_name);
}

std::string JGINYUEInternalUSBV2Controller::GetSerialString()
{
    return("");
}

std::string JGINYUEInternalUSBV2Controller::GetDeviceFWVersion()
{


    return(Major_version + "." + Minor_version);
}

void JGINYUEInternalUSBV2Controller::Init_device()
{
    unsigned char   usb_buf[64];

    memset(usb_buf, 0x00, sizeof(usb_buf));  

}

void JGINYUEInternalUSBV2Controller::WriteZoneMode
    (
    unsigned char   zone,
    unsigned char   mode,
    unsigned char   num_colors,
    RGBColor        rgb[10],
    unsigned char   speed,
    unsigned char   brightness,
    unsigned char   direction
    )
{

}

void JGINYUEInternalUSBV2Controller::DirectLEDControl
    (
    RGBColor*              colors,
    unsigned char          zone
    )
{

}

void JGINYUEInternalUSBV2Controller::Area_resize(unsigned char led_numbers, unsigned char zone)
{

}

void JGINYUEInternalUSBV2Controller::SetRGSwap(unsigned char RGSwap)
{

}
