/*---------------------------------------------------------*\
| ResourceManager.h                                         |
|                                                           |
|   OpenRGB Resource Manager controls access to application |
|   components including RGBControllers, I2C interfaces,    |
|   and network SDK components                              |
|                                                           |
|   Adam Honse (CalcProgrammer1)                27 Sep 2020 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include "DeviceDetector.h"
#include "filesystem.h"
#include "i2c_smbus.h"
#include "hidapi_wrapper.h"

#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <string>

struct hid_device_info;
class NetworkClient;
class NetworkServer;
class ProfileManager;
class RGBController;
class SettingsManager;
struct hid_device_info;
typedef std::vector<RGBController*> Controllers;

class BasicHIDBlock
{
public:
    std::string  name;
    uint16_t     vid;
    uint16_t     pid;
    int          interface;
    int          usage_page;
    int          usage;
    bool         compare(hid_device_info*);
};

struct HIDDeviceDetectorBlock : public BasicHIDBlock
{
    HIDDeviceDetectorFunction function;
};

struct HIDWrappedDeviceDetectorBlock : public BasicHIDBlock
{
    HIDWrappedDeviceDetectorFunction function;
};

typedef struct
{
    std::string                     name;
    I2CPCIDeviceDetectorFunction    function;
    uint16_t                        ven_id;
    uint16_t                        dev_id;
    uint16_t                        subven_id;
    uint16_t                        subdev_id;
    uint8_t                         i2c_addr;
} I2CPCIDeviceDetectorBlock;

typedef void (*DeviceListChangeCallback)(void *);
typedef void (*DetectionProgressCallback)(void *);
typedef void (*DetectionStartCallback)(void *);
typedef void (*DetectionEndCallback)(void *);
typedef void (*I2CBusListChangeCallback)(void *);

class ResourceManagerInterface
{
public:
    virtual std::vector<i2c_smbus_interface*> & GetI2CBusses()                                                                                      = 0;

    virtual void                                RegisterRGBController(RGBController *rgb_controller)                                                = 0;
    virtual void                                UnregisterRGBController(RGBController *rgb_controller)                                              = 0;

    virtual void                                RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg)    = 0;
    virtual void                                RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg)  = 0;
    virtual void                                RegisterDetectionStartCallback(DetectionStartCallback new_callback, void * new_callback_arg)        = 0;
    virtual void                                RegisterDetectionEndCallback(DetectionEndCallback new_callback, void * new_callback_arg)            = 0;
    virtual void                                RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg)    = 0;

    virtual void                                UnregisterDeviceListChangeCallback(DeviceListChangeCallback callback, void * callback_arg)          = 0;
    virtual void                                UnregisterDetectionProgressCallback(DetectionProgressCallback callback, void *callback_arg)         = 0;
    virtual void                                UnregisterDetectionStartCallback(DetectionStartCallback callback, void *callback_arg)               = 0;
    virtual void                                UnregisterDetectionEndCallback(DetectionEndCallback callback, void *callback_arg)                   = 0;
    virtual void                                UnregisterI2CBusListChangeCallback(I2CBusListChangeCallback callback, void * callback_arg)          = 0;

    virtual std::vector<RGBController*> &       GetRGBControllers()                                                                                 = 0;

    virtual unsigned int                        GetDetectionPercent()                                                                               = 0;

    virtual filesystem::path                    GetConfigurationDirectory()                                                                         = 0;

    virtual std::vector<NetworkClient*>&        GetClients()                                                                                        = 0;
    virtual NetworkServer*                      GetServer()                                                                                         = 0;

    virtual ProfileManager*                     GetProfileManager()                                                                                 = 0;
    virtual SettingsManager*                    GetSettingsManager()                                                                                = 0;

    virtual void                                UpdateDeviceList()                                                                                  = 0;
    virtual void                                WaitForDeviceDetection()                                                                            = 0;

protected:
    virtual                                    ~ResourceManagerInterface() {};
};

class ResourceManager: public ResourceManagerInterface
{
public:
    static ResourceManager *get();

    ResourceManager();
    ~ResourceManager();

    void RegisterI2CBus(i2c_smbus_interface *);
    std::vector<i2c_smbus_interface*> & GetI2CBusses();

    void RegisterRGBController(RGBController *rgb_controller);
    void UnregisterRGBController(RGBController *rgb_controller);

    std::vector<RGBController*> & GetRGBControllers();

    void RegisterI2CBusDetector         (I2CBusDetectorFunction     detector);
    void RegisterDeviceDetector         (std::string name, DeviceDetectorFunction     detector);
    void RegisterI2CDeviceDetector      (std::string name, I2CDeviceDetectorFunction  detector);
    void RegisterI2CPCIDeviceDetector   (std::string name, I2CPCIDeviceDetectorFunction detector, uint16_t ven_id, uint16_t dev_id, uint16_t subven_id, uint16_t subdev_id, uint8_t i2c_addr);

    void RegisterHIDDeviceDetector(std::string name,
                                   HIDDeviceDetectorFunction  detector,
                                   uint16_t vid,
                                   uint16_t pid,
                                   int interface,
                                   int usage_page,
                                   int usage);
    void RegisterHIDWrappedDeviceDetector   (std::string name,
                                            HIDWrappedDeviceDetectorFunction  detector,
                                            uint16_t vid,
                                            uint16_t pid,
                                            int interface,
                                            int usage_page,
                                            int usage);

    void RegisterDynamicDetector        (std::string name, DynamicDetectorFunction detector);
    void RegisterPreDetectionHook       (PreDetectionHookFunction hook);

    void RegisterDeviceListChangeCallback(DeviceListChangeCallback new_callback, void * new_callback_arg);
    void RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * new_callback_arg);
    void RegisterDetectionStartCallback(DetectionStartCallback new_callback, void * new_callback_arg);
    void RegisterDetectionEndCallback(DetectionEndCallback new_callback, void * new_callback_arg);
    void RegisterI2CBusListChangeCallback(I2CBusListChangeCallback new_callback, void * new_callback_arg);

    void UnregisterDeviceListChangeCallback(DeviceListChangeCallback callback, void * callback_arg);
    void UnregisterDetectionProgressCallback(DetectionProgressCallback callback, void *callback_arg);
    void UnregisterDetectionStartCallback(DetectionStartCallback callback, void *callback_arg);
    void UnregisterDetectionEndCallback(DetectionEndCallback callback, void *callback_arg);
    void UnregisterI2CBusListChangeCallback(I2CBusListChangeCallback callback, void * callback_arg);

    bool         GetDetectionEnabled();
    unsigned int GetDetectionPercent();
    const char*  GetDetectionString();

    filesystem::path                GetConfigurationDirectory();

    void RegisterNetworkClient(NetworkClient* new_client);
    void UnregisterNetworkClient(NetworkClient* network_client);

    std::vector<NetworkClient*>&    GetClients();
    NetworkServer*                  GetServer();

    ProfileManager*                 GetProfileManager();
    SettingsManager*                GetSettingsManager();

    void                            SetConfigurationDirectory(const filesystem::path &directory);

    void ProcessPreDetectionHooks(); // Consider making private
    void ProcessDynamicDetectors();  // Consider making private
    void UpdateDeviceList();
    void DeviceListChanged();
    void DetectionProgressChanged();
    void I2CBusListChanged();

    void Initialize(bool tryConnect, bool detectDevices, bool startServer, bool applyPostOptions);

    void Cleanup();

    void DetectDevices();

    void DisableDetection();

    void StopDeviceDetection();

    void WaitForInitialization();
    void WaitForDeviceDetection();

private:
    void DetectDevicesThreadFunction();
    void UpdateDetectorSettings();
    void SetupConfigurationDirectory();
    bool AttemptLocalConnection();
    void InitThreadFunction();
    bool ProcessPreDetection();
    void ProcessPostDetection();

    void RunHidDetector(hid_device_info* info, bool detection);
    void RunWrappedHidDetector(const hidapi_wrapper* wrapper, hid_device_info* info, bool detection);

    void StartHotplug();
    void StopHotplug();

#if(HOTPLUG_ENABLED)
    static int HotplugCallbackFunction(hid_hotplug_callback_handle callback_handle,
                                        hid_device_info *device,
                                        hid_hotplug_event event,
                                        void *user_data);
    static int HotplugLibusbCallbackFunction(hid_hotplug_callback_handle callback_handle,
                                       hid_device_info *device,
                                       hid_hotplug_event event,
                                       void *user_data);
    static int UnplugCallbackFunction(hid_hotplug_callback_handle callback_handle,
                                       hid_device_info *device,
                                       hid_hotplug_event event,
                                       void *user_data);
#endif

    /*-------------------------------------------------------------------------------------*\
    | Static pointer to shared instance of ResourceManager                                  |
    \*-------------------------------------------------------------------------------------*/
    static ResourceManager*                     instance;

    /*-------------------------------------------------------------------------------------*\
    | Auto connection permitting flag                                                       |
    \*-------------------------------------------------------------------------------------*/
    bool                                        tryAutoConnect;

    /*-------------------------------------------------------------------------------------*\
    | Detection enabled flag                                                                |
    \*-------------------------------------------------------------------------------------*/
    bool                                        detection_enabled;

    /*-------------------------------------------------------------------------------------*\
    | Auto connection permitting flag                                                       |
    \*-------------------------------------------------------------------------------------*/
    bool                                        start_server;

    /*-------------------------------------------------------------------------------------*\
    | Auto connection permitting flag                                                       |
    \*-------------------------------------------------------------------------------------*/
    bool                                        apply_post_options;
    /*-------------------------------------------------------------------------------------*\
    | Initialization completion flag                                                        |
    \*-------------------------------------------------------------------------------------*/
    std::atomic<bool>                           init_finished;

    /*-------------------------------------------------------------------------------------*\
    | Profile Manager                                                                       |
    \*-------------------------------------------------------------------------------------*/
    ProfileManager*                             profile_manager;

    /*-------------------------------------------------------------------------------------*\
    | Settings Manager                                                                      |
    \*-------------------------------------------------------------------------------------*/
    SettingsManager*                            settings_manager;

    /*-------------------------------------------------------------------------------------*\
    | I2C/SMBus Interfaces                                                                  |
    \*-------------------------------------------------------------------------------------*/
    std::vector<i2c_smbus_interface*>           busses;

    /*-------------------------------------------------------------------------------------*\
    | RGBControllers                                                                        |
    \*-------------------------------------------------------------------------------------*/
    std::vector<RGBController*>                 rgb_controllers_sizes;
    std::vector<RGBController*>                 rgb_controllers_hw;
    std::vector<RGBController*>                 rgb_controllers;

    /*-------------------------------------------------------------------------------------*\
    | Network Server                                                                        |
    \*-------------------------------------------------------------------------------------*/
    NetworkServer*                              server;

    /*-------------------------------------------------------------------------------------*\
    | Network Clients                                                                       |
    \*-------------------------------------------------------------------------------------*/
    std::vector<NetworkClient*>                 clients;

    /*-------------------------------------------------------------------------------------*\
    | Detectors                                                                             |
    \*-------------------------------------------------------------------------------------*/
    std::vector<DeviceDetectorFunction>         device_detectors;
    std::vector<std::string>                    device_detector_strings;
    std::vector<I2CBusDetectorFunction>         i2c_bus_detectors;
    std::vector<I2CDeviceDetectorFunction>      i2c_device_detectors;
    std::vector<std::string>                    i2c_device_detector_strings;
    std::vector<I2CPCIDeviceDetectorBlock>      i2c_pci_device_detectors;
    std::vector<HIDDeviceDetectorBlock>         hid_device_detectors;
    std::vector<HIDWrappedDeviceDetectorBlock>  hid_wrapped_device_detectors;
    std::vector<DynamicDetectorFunction>        dynamic_detectors;
    std::vector<std::string>                    dynamic_detector_strings;
    std::vector<PreDetectionHookFunction>       pre_detection_hooks;

    bool                                        dynamic_detectors_processed;

    /*-------------------------------------------------------------------------------------*\
    | Hotplug handles                                                                       |
    \*-------------------------------------------------------------------------------------*/
#if(HOTPLUG_ENABLED)
    hid_hotplug_callback_handle                 hotplug_callback_handle;
    hid_hotplug_callback_handle                 libusb_hotplug_callback_handle;
#endif
    bool                                        hotplug_enabled;
    bool                                        hotplug_libusb_enabled;

    /*-------------------------------------------------------------------------------------*\
    | Detection Thread and Detection State                                                  |
    \*-------------------------------------------------------------------------------------*/
    std::thread *                               DetectDevicesThread; // Used for rescan
    std::thread *                               InitThread; // Used for initial scan, initial network scan, server startup
    std::mutex                                  DetectDeviceMutex;

    std::atomic<bool>                           detection_is_required;
    std::atomic<unsigned int>                   detection_percent;
    std::atomic<unsigned int>                   detection_prev_size;
    std::vector<bool>                           detection_size_entry_used;
    const char*                                 detection_string;


    /*-------------------------------------------------------------------------------------*\
    | Device List Changed Callback                                                          |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DeviceListChangeMutex;
    std::vector<DeviceListChangeCallback>       DeviceListChangeCallbacks;
    std::vector<void *>                         DeviceListChangeCallbackArgs;

    /*-------------------------------------------------------------------------------------*\
    | Detection Progress, Start, and End Callbacks                                          |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DetectionProgressMutex;
    std::vector<DetectionProgressCallback>      DetectionProgressCallbacks;
    std::vector<void *>                         DetectionProgressCallbackArgs;

    std::vector<DetectionStartCallback>         DetectionStartCallbacks;
    std::vector<void *>                         DetectionStartCallbackArgs;

    std::vector<DetectionEndCallback>           DetectionEndCallbacks;
    std::vector<void *>                         DetectionEndCallbackArgs;

    /*-------------------------------------------------------------------------------------*\
    | I2C/SMBus Adapter List Changed Callback                                               |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  I2CBusListChangeMutex;
    std::vector<I2CBusListChangeCallback>       I2CBusListChangeCallbacks;
    std::vector<void *>                         I2CBusListChangeCallbackArgs;

    filesystem::path config_dir;
};
