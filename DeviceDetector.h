/*---------------------------------------------------------*\
| DeviceDetector.h                                          |
|                                                           |
|   Device detector functionality                           |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include <functional>
#include <string>
#include <vector>
#include <stdint.h>

#include "hidapi_wrapper/hidapi_wrapper.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1

struct hid_device_info;
class i2c_smbus_interface;
class RGBController;
typedef std::vector<RGBController*> Controllers;

typedef std::function<bool()>                                                               I2CBusDetectorFunction;
typedef std::function<void()>                                                               DeviceDetectorFunction;
typedef std::function<void(std::vector<i2c_smbus_interface*>&)>                             I2CDeviceDetectorFunction;
typedef std::function<void(i2c_smbus_interface*, uint8_t, const std::string&)>              I2CPCIDeviceDetectorFunction;

typedef std::function<Controllers(hid_device_info*, const std::string&)>                    HIDDeviceDetectorFunction;
typedef std::function<Controllers(const hidapi_wrapper* wrapper, hid_device_info*, const std::string&)> HIDWrappedDeviceDetectorFunction;
typedef std::function<void()>                                                               DynamicDetectorFunction;
typedef std::function<void()>                                                               PreDetectionHookFunction;


class DeviceDetector
{
public:
    DeviceDetector(std::string name, DeviceDetectorFunction detector);
};

class I2CDeviceDetector
{
public:
    I2CDeviceDetector(std::string name, I2CDeviceDetectorFunction detector);
};

class I2CPCIDeviceDetector
{
public:
    I2CPCIDeviceDetector(std::string name, I2CPCIDeviceDetectorFunction detector, uint16_t ven_id, uint16_t dev_id, uint16_t subven_id, uint16_t subdev_id, uint8_t i2c_addr);
};

class I2CBusDetector
{
public:
    I2CBusDetector(I2CBusDetectorFunction detector);
};

class HIDDeviceDetector
{
public:
    HIDDeviceDetector(std::string name, HIDDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage);
};

class HIDWrappedDeviceDetector
{
public:
    HIDWrappedDeviceDetector(std::string name, HIDWrappedDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage);
};

class DynamicDetector
{
public:
    DynamicDetector(std::string name, DynamicDetectorFunction detector);
};

class PreDetectionHook
{
public:
    PreDetectionHook(PreDetectionHookFunction hook);
};
