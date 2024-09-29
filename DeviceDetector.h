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

#include "DeviceType.h"
#include "ResourceManager.h"

class DeviceDetector
{
public:
    DeviceDetector(std::string name, device_type type, DeviceDetectorFunction detector)
	{
        ResourceManager::get()->RegisterDeviceDetector(name, type, detector);
	}
};

class I2CDeviceDetector
{
public:
    I2CDeviceDetector(std::string name, device_type type, I2CDeviceDetectorFunction detector)
	{
        ResourceManager::get()->RegisterI2CDeviceDetector(name, type, detector);
	}
};

class I2CPCIDeviceDetector
{
public:
    I2CPCIDeviceDetector(std::string name, device_type type, I2CPCIDeviceDetectorFunction detector, uint16_t ven_id, uint16_t dev_id, uint16_t subven_id, uint16_t subdev_id, uint8_t i2c_addr)
    {
        ResourceManager::get()->RegisterI2CPCIDeviceDetector(name, type, detector, ven_id, dev_id, subven_id, subdev_id, i2c_addr);
    }
};

class I2CBusDetector
{
public:
    I2CBusDetector(I2CBusDetectorFunction detector)
    {
        ResourceManager::get()->RegisterI2CBusDetector(detector);
    }
};

class HIDDeviceDetector
{
public:
    HIDDeviceDetector(std::string name, device_type type, HIDDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage)
    {
        ResourceManager::get()->RegisterHIDDeviceDetector(name, type, detector, vid, pid, interface, usage_page, usage);
    }
};

class HIDWrappedDeviceDetector
{
public:
    HIDWrappedDeviceDetector(std::string name, device_type type, HIDWrappedDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage)
    {
        ResourceManager::get()->RegisterHIDWrappedDeviceDetector(name, type, detector, vid, pid, interface, usage_page, usage);
    }
};

class DynamicDetector
{
public:
    DynamicDetector(std::string name, device_type type, DynamicDetectorFunction detector)
    {
        ResourceManager::get()->RegisterDynamicDetector(name, type, detector);
    }
};

class PreDetectionHook
{
public:
    PreDetectionHook(PreDetectionHookFunction hook)
    {
        ResourceManager::get()->RegisterPreDetectionHook(hook);
    }
};
