#pragma once

#include "DeviceDetector.h"

#define REGISTER_DETECTOR(name, func)                                              static DeviceDetector device_detector_obj_##func(name, func)
#define REGISTER_I2C_DETECTOR(name, func)                                          static I2CDeviceDetector device_detector_obj_##func(name, func)
#define REGISTER_I2C_BUS_DETECTOR(func)                                            static I2CBusDetector device_detector_obj_##func(func)
#define REGISTER_HID_DETECTOR(name, func, vid, pid)                                static HIDDeviceDetector device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_I(name, func, vid, pid, interface_no)                static HIDDeviceDetector device_detector_obj_##vid##pid##_##interface_no(name, func, vid, pid, interface_no, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IP(name, func, vid, pid, interface_no, page)         static HIDDeviceDetector device_detector_obj_##vid##pid##_##interface_no##_##page(name, func, vid, pid, interface_no, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IPU(name, func, vid, pid, interface_no, page, usage) static HIDDeviceDetector device_detector_obj_##vid##pid##_##interface_no##_##page##_##usage(name, func, vid, pid, interface_no, page, usage)
#define REGISTER_HID_DETECTOR_P(name, func, vid, pid, page)                        static HIDDeviceDetector device_detector_obj_##vid##pid##__##page(name, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_PU(name, func, vid, pid, page, usage)                static HIDDeviceDetector device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)
