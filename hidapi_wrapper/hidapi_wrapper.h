/*---------------------------------------------------------*\
| hidapi_wrapper.h                                          |
|                                                           |
|   Wrapper for hidapi that can select from default or      |
|   libusb backends on Linux                                |
|                                                           |
|   Matt Silva (thesilvanator)                  2022        |
|   Adam Honse (CalcProgrammer1)                2023        |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

// Hotplug implementation can be enabled or disabled at compile time
// If it is not available on the target system, it will be blocked anyway
#ifndef HOTPLUG_ENABLED
#define HOTPLUG_ENABLED 1
#endif

#if(HOTPLUG_ENABLED)
// NOTE: This has to be the local version of the header
// Otherwise, we get no declarations for the hotplug calls
#include "dependencies/hidapi-hotplug/hidapi/hidapi.h"
#else
// NOTE: This is the system header
#include <hidapi.h>
#endif

#ifdef __linux__
#include <dlfcn.h>
#endif



/*-----------------------------------------------------*\
| Type definitions for libhidapi function pointers      |
\*-----------------------------------------------------*/
typedef int                 (*hidapi_wrapper_send_feature_report)   (hid_device*, const unsigned char*, size_t);
typedef int                 (*hidapi_wrapper_get_feature_report)    (hid_device*, unsigned char*, size_t);
typedef int                 (*hidapi_wrapper_get_serial_number_string) (hid_device*, wchar_t*, size_t);
typedef hid_device*         (*hidapi_wrapper_open_path)             (const char*);
typedef hid_device_info*    (*hidapi_wrapper_enumerate)             (unsigned short, unsigned short);
typedef void                (*hidapi_wrapper_free_enumeration)      (hid_device_info*);
typedef void                (*hidapi_wrapper_close)                 (hid_device*);
typedef const wchar_t*      (*hidapi_wrapper_error)                 (hid_device*);

#if(HOTPLUG_ENABLED)
typedef int                 (*hidapi_wrapper_hotplug_register_callback)(unsigned short vendor_id, unsigned short product_id, int events, int flags, hid_hotplug_callback_fn callback, void* user_data, hid_hotplug_callback_handle* callback_handle);
typedef int                 (*hidapi_wrapper_hotplug_deregister_callback)(hid_hotplug_callback_handle callback_handle);
#endif

/*-----------------------------------------------------*\
|  See comment at top of HyperXQuadcastSDetect.cpp for  |
|  details about the hidapi wrapper for this device     |
\*-----------------------------------------------------*/
struct hidapi_wrapper
{
    void*                                   dyn_handle;
    hidapi_wrapper_send_feature_report      hid_send_feature_report;
    hidapi_wrapper_get_feature_report       hid_get_feature_report;
    hidapi_wrapper_get_serial_number_string hid_get_serial_number_string;
    hidapi_wrapper_open_path                hid_open_path;
    hidapi_wrapper_enumerate                hid_enumerate;
    hidapi_wrapper_free_enumeration         hid_free_enumeration;
    hidapi_wrapper_close                    hid_close;
    hidapi_wrapper_error                    hid_error;
#if(HOTPLUG_ENABLED)
    hidapi_wrapper_hotplug_register_callback hid_hotplug_register_callback;
    hidapi_wrapper_hotplug_deregister_callback hid_hotplug_deregister_callback;
#endif
    const char* name;
};
