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

#define REGISTER_DETECTOR(name, type, func)                                                   static DeviceDetector           device_detector_obj_##func(name, type, func)
#define REGISTER_I2C_DETECTOR(name, type, func)                                               static I2CDeviceDetector        device_detector_obj_##func(name, type, func)
#define REGISTER_I2C_PCI_DETECTOR(name, type, func, ven, dev, subven, subdev, addr)           static I2CPCIDeviceDetector     device_detector_obj_##ven##dev##subven##subdev##addr##func(name, type, func, ven, dev, subven, subdev, addr)
#define REGISTER_I2C_BUS_DETECTOR(func)                                                       static I2CBusDetector           device_detector_obj_##func(func)
#define REGISTER_HID_DETECTOR(name, type, func, vid, pid)                                     static HIDDeviceDetector        device_detector_obj_##vid##pid(name, type, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_I(name, type, func, vid, pid, interface)                        static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface(name, type, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IP(name, type, func, vid, pid, interface, page)                 static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface##_##page(name, type, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IPU(name, type, func, vid, pid, interface, page, usage)         static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, type, func, vid, pid, interface, page, usage)
#define REGISTER_HID_DETECTOR_P(name, type, func, vid, pid, page)                             static HIDDeviceDetector        device_detector_obj_##vid##pid##__##page(name, type, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_PU(name, type, func, vid, pid, page, usage)                     static HIDDeviceDetector        device_detector_obj_##vid##pid##__##page##_##usage(name, type, func, vid, pid, HID_INTERFACE_ANY, page, usage)
#define REGISTER_HID_WRAPPED_DETECTOR(name, type, func, vid, pid)                             static HIDWrappedDeviceDetector device_detector_obj_##vid##pid(name, type, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_WRAPPED_DETECTOR_I(name, type, func, vid, pid, interface)                static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##_##interface(name, type, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_WRAPPED_DETECTOR_IPU(name, type, func, vid, pid, interface, page, usage) static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, type, func, vid, pid, interface, page, usage)
#define REGISTER_HID_WRAPPED_DETECTOR_PU(name, type, func, vid, pid, page, usage)             static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##__##page##_##usage(name, type, func, vid, pid, HID_INTERFACE_ANY, page, usage)
#define REGISTER_DYNAMIC_DETECTOR(name, type, func)                                           static DynamicDetector          device_detector_obj_##func(name, type, func)
#define REGISTER_PRE_DETECTION_HOOK(func)                                                     static PreDetectionHook         device_detector_obj_##func(func)

#define REGISTER_DYNAMIC_I2C_DETECTOR(name, type, func)                                       I2CDeviceDetector               device_detector_obj_##func(name, type, func)
#define REGISTER_DYNAMIC_I2C_PCI_DETECTOR(name, type, func, ven, dev, subven, subdev, addr)   I2CPCIDeviceDetector            device_detector_obj_##ven##dev##subven##subdev##addr##func(name, type, func, ven, dev, subven, subdev, addr)
#define REGISTER_DYNAMIC_I2C_BUS_DETECTOR(func)                                               I2CBusDetector                  device_detector_obj_##func(func)
#define REGISTER_DYNAMIC_HID_DETECTOR(name, type, func, vid, pid)                             HIDDeviceDetector               device_detector_obj_##vid##pid(name, type, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_I(name, type, func, vid, pid, interface)                HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface(name, type, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IP(name, type, func, vid, pid, interface, page)         HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface##_##page(name, type, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IPU(name, type, func, vid, pid, interface, page, usage) HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, type, func, vid, pid, interface, page, usage)
#define REGISTER_DYNAMIC_HID_DETECTOR_P(name, type, func, vid, pid, page)                     HIDDeviceDetector               device_detector_obj_##vid##pid##__##page(name, type, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_PU(name, type, func, vid, pid, page, usage)             HIDDeviceDetector               device_detector_obj_##vid##pid##__##page##_##usage(name, type, func, vid, pid, HID_INTERFACE_ANY, page, usage)
