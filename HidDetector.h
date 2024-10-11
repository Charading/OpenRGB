#pragma once

#include "DeviceDetector.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1

#define REGISTER_HID_DETECTOR(name, func, vid, pid)                                     static HIDDeviceDetector        device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_I(name, func, vid, pid, interface)                        static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface(name, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IP(name, func, vid, pid, interface, page)                 static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface##_##page(name, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_IPU(name, func, vid, pid, interface, page, usage)         static HIDDeviceDetector        device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, func, vid, pid, interface, page, usage)
#define REGISTER_HID_DETECTOR_P(name, func, vid, pid, page)                             static HIDDeviceDetector        device_detector_obj_##vid##pid##__##page(name, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_HID_DETECTOR_PU(name, func, vid, pid, page, usage)                     static HIDDeviceDetector        device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)

#define REGISTER_HID_WRAPPED_DETECTOR(name, func, vid, pid)                             static HIDWrappedDeviceDetector device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_WRAPPED_DETECTOR_I(name, func, vid, pid, interface)                static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##_##interface(name, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_HID_WRAPPED_DETECTOR_IPU(name, func, vid, pid, interface, page, usage) static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, func, vid, pid, interface, page, usage)
#define REGISTER_HID_WRAPPED_DETECTOR_PU(name, func, vid, pid, page, usage)             static HIDWrappedDeviceDetector device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)

#define REGISTER_DYNAMIC_HID_DETECTOR(name, func, vid, pid)                             HIDDeviceDetector               device_detector_obj_##vid##pid(name, func, vid, pid, HID_INTERFACE_ANY, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_I(name, func, vid, pid, interface)                HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface(name, func, vid, pid, interface, HID_USAGE_PAGE_ANY, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IP(name, func, vid, pid, interface, page)         HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface##_##page(name, func, vid, pid, interface, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_IPU(name, func, vid, pid, interface, page, usage) HIDDeviceDetector               device_detector_obj_##vid##pid##_##interface##_##page##_##usage(name, func, vid, pid, interface, page, usage)
#define REGISTER_DYNAMIC_HID_DETECTOR_P(name, func, vid, pid, page)                     HIDDeviceDetector               device_detector_obj_##vid##pid##__##page(name, func, vid, pid, HID_INTERFACE_ANY, page, HID_USAGE_ANY)
#define REGISTER_DYNAMIC_HID_DETECTOR_PU(name, func, vid, pid, page, usage)             HIDDeviceDetector               device_detector_obj_##vid##pid##__##page##_##usage(name, func, vid, pid, HID_INTERFACE_ANY, page, usage)

#define GENERIC_HOTPLUGGABLE_DETECTOR(name, c_controller, c_rgbcontroller) \
static Controllers name(hid_device_info *info, const std::string &) \
{ \
        Controllers result; \
        hid_device* dev = hid_open_path(info->path); \
        if(dev) \
    { \
            c_controller* c    = new c_controller(dev, info->path); \
            c_rgbcontroller* r = new c_rgbcontroller(c); \
            result.push_back(r); \
    } \
        return result; \
}
