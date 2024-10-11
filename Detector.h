/*---------------------------------------------------------*\
| Detector.h                                                |
|                                                           |
|   Macros for registering detectors                        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include "DeviceDetector.h"
#include "ResourceManager.h"

#define REGISTER_DETECTOR(name, func)                                                   static DeviceDetector           device_detector_obj_##func(name, func)
#define REGISTER_DYNAMIC_DETECTOR(name, func)                                           static DynamicDetector          device_detector_obj_##func(name, func)
#define REGISTER_PRE_DETECTION_HOOK(func)                                               static PreDetectionHook         device_detector_obj_##func(func)

#define REGISTER_I2C_DETECTOR(name, func)                                               static I2CDeviceDetector        device_detector_obj_##func(name, func)
#define REGISTER_I2C_PCI_DETECTOR(name, func, ven, dev, subven, subdev, addr)           static I2CPCIDeviceDetector     device_detector_obj_##ven##dev##subven##subdev##addr##func(name, func, ven, dev, subven, subdev, addr)
#define REGISTER_I2C_BUS_DETECTOR(func)                                                 static I2CBusDetector           device_detector_obj_##func(func)

#define REGISTER_DYNAMIC_I2C_DETECTOR(name, func)                                       I2CDeviceDetector               device_detector_obj_##func(name, func)
#define REGISTER_DYNAMIC_I2C_PCI_DETECTOR(name, func, ven, dev, subven, subdev, addr)   I2CPCIDeviceDetector            device_detector_obj_##ven##dev##subven##subdev##addr##func(name, func, ven, dev, subven, subdev, addr)
#define REGISTER_DYNAMIC_I2C_BUS_DETECTOR(func)                                         I2CBusDetector                  device_detector_obj_##func(func)
