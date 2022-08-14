#pragma once

#include "DeviceDetector.h"

#define REGISTER_DETECTOR(name, func)                                            static DeviceDetector       device_detector_obj_##func(name, func)
#define REGISTER_I2C_DETECTOR(name, func)                                        static I2CDeviceDetector    device_detector_obj_##func(name, func)
#define REGISTER_I2C_PCI_DETECTOR(name, func, ven, dev, subven, subdev, addr)    static I2CPCIDeviceDetector device_detector_obj_##ven##dev##subven##subdev##addr##func(name, func, ven, dev, subven, subdev, addr)
#define REGISTER_I2C_BUS_DETECTOR(func)                                          static I2CBusDetector       device_detector_obj_##func(func)
#define REGISTER_HID_DETECTOR(name, func, vid, pid)                              static HIDDeviceDetector    device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_I(name, func, vid, pid, interface)                 static HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface(name, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IP(name, func, vid, pid, interface, page)          static HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface##_##page(name, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IPU(name, func, vid, pid, interface, page, usage)  static HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, func, vid, pid, interface, page, usage)
#define REGISTER_HID_DETECTOR_P(name, func, vid, pid, page)                      static HIDDeviceDetector    device_detector_obj_##vid##pid##__##page(name, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_PU(name, func, vid, pid, page, usage)              static HIDDeviceDetector    device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)

#ifdef USE_HID_USAGE
#   define REGISTER_HID_DETECTOR_I_ONLY(name, func, interface)                      static HIDDeviceDetector    device_detector_obj_##interface(name, func, HID_VID_ANY, HID_PID_ANY, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#   define REGISTER_HID_DETECTOR_IP_ONLY(name, func, interface, page)               static HIDDeviceDetector    device_detector_obj_##interface##_##page(name, func, HID_VID_ANY, HID_PID_ANY, interface, page, HID_USAGE_ANY)
#   define REGISTER_HID_DETECTOR_IPU_ONLY(name, func, interface, page, usage)       static HIDDeviceDetector    device_detector_obj_##interface##_##page##_##usage(name, func, HID_VID_ANY, HID_PID_ANY, interface, page, usage)
#   define REGISTER_HID_DETECTOR_P_ONLY(name, func, page)                           static HIDDeviceDetector    device_detector_obj__##page(name, func, HID_VID_ANY, HID_PID_ANY, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#   define REGISTER_HID_DETECTOR_PU_ONLY(name, func, page, usage)                   static HIDDeviceDetector    device_detector_obj__##page##_##usage(name, func, HID_VID_ANY, HID_PID_ANY, HID_INTERFACE_ANY, page, usage)
#endif

#define REGISTER_DYNAMIC_DETECTOR(name, func)                                    static DynamicDetector      device_detector_obj_##func(name, func)
#define REGISTER_PRE_DETECTION_HOOK(func)                                        static PreDetectionHook     device_detector_obj_##func(func)

#define REGISTER_DYNAMIC_I2C_DETECTOR(name, func)                                       I2CDeviceDetector    device_detector_obj_##func(name, func)
#define REGISTER_DYNAMIC_I2C_PCI_DETECTOR(name, func, ven, dev, subven, subdev, addr)   I2CPCIDeviceDetector device_detector_obj_##ven##dev##subven##subdev##addr##func(name, func, ven, dev, subven, subdev, addr)
#define REGISTER_DYNAMIC_I2C_BUS_DETECTOR(func)                                         I2CBusDetector       device_detector_obj_##func(func)
#define REGISTER_DYNAMIC_HID_DETECTOR(name, func, vid, pid)                             HIDDeviceDetector    device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_I(name, func, vid, pid, interface)                HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface(name, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IP(name, func, vid, pid, interface, page)         HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface##_##page(name, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IPU(name, func, vid, pid, interface, page, usage) HIDDeviceDetector    device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, func, vid, pid, interface, page, usage)
#define REGISTER_DYNAMIC_HID_DETECTOR_P(name, func, vid, pid, page)                     HIDDeviceDetector    device_detector_obj_##vid##pid##__##page(name, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_PU(name, func, vid, pid, page, usage)             HIDDeviceDetector    device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)
