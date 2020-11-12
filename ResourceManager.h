/*-----------------------------------------*\
|  ResourceManager.h                        |
|                                           |
|  OpenRGB Resource Manager controls access |
|  to application components including      |
|  RGBControllers, I2C interfaces, and      |
|  network SDK components                   |
|                                           |
|  Adam Honse (CalcProgrammer1) 9/27/2020   |
\*-----------------------------------------*/

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <thread>
#include <string>

#include "i2c_smbus.h"
#include "NetworkClient.h"
#include "NetworkServer.h"
#include "ProfileManager.h"
#include "RGBController.h"
#include "SettingsManager.h"

#define HID_INTERFACE_ANY   -1
#define HID_USAGE_ANY       -1
#define HID_USAGE_PAGE_ANY  -1

struct hid_device_info;

typedef std::function<void(std::vector<i2c_smbus_interface*>&)>                                 I2CBusDetectorFunction;
typedef std::function<void(std::vector<RGBController*>&)>                                       DeviceDetectorFunction;
typedef std::function<void(std::vector<i2c_smbus_interface*>&, std::vector<RGBController*>&)>   I2CDeviceDetectorFunction;
typedef std::function<void(hid_device_info*, const std::string&)>                               HIDDeviceDetectorFunction;

typedef struct
{
    std::string                 name;
    HIDDeviceDetectorFunction   function;
    unsigned int                address;
    int                         interface;
    int                         usage_page;
    int                         usage;
} HIDDeviceDetectorBlock;

enum ControllerList
{
    CONTROLLER_LIST_UI = 0, // Merges all lists
    CONTROLLER_LIST_HW = 1, // Merges all hardware lists
    CONTROLLER_LIST_I2C,
    CONTROLLER_LIST_HID,
    CONTROLLER_LIST_USB, // None implemented yet
    CONTROLLER_LIST_MISC,
    CONTROLLER_LIST_REMOTE = 20
};

typedef void (*DeviceListChangeCallback)(void* receiver, int index, ControllerList list, bool removed);
typedef void (*DetectionProgressCallback)(void* receiver);
typedef std::pair<DeviceListChangeCallback, void*> DeviceListChangeBlock;
typedef std::pair<DetectionProgressCallback, void*> DetectionProgressBlock;

class ResourceManager
{
public:
    static ResourceManager *get();
    
    ResourceManager();
    ~ResourceManager();
    
    void RegisterI2CBus(i2c_smbus_interface *);
    std::vector<i2c_smbus_interface*> & GetI2CBusses();

    // Add a controller to the specified list
    bool           RegisterRGBController(RGBController*, ControllerList list);
    // Remove a controller from internal lists; Does not delete the controller object!
    bool           RemoveRGBController(RGBController*, ControllerList list = CONTROLLER_LIST_UI);
    size_t         GetControllerCount(ControllerList list = CONTROLLER_LIST_UI) const;
    RGBController* GetController(size_t, ControllerList list = CONTROLLER_LIST_UI);
    int            GetUIListIndex(size_t, ControllerList list) const;
    size_t         GetDetectorCount(ControllerList list = CONTROLLER_LIST_HW) const;
    const char*    GetDetectorName(size_t index, ControllerList list = CONTROLLER_LIST_HW) const;
    bool           IsDetectorEnabled(size_t index, ControllerList list = CONTROLLER_LIST_HW) const;
    bool           IsDetectorEnabled(const char* name) const;
    bool           SetDetectorEnabled(bool en, size_t index, ControllerList list = CONTROLLER_LIST_HW);
    bool           SetDetectorEnabled(bool en, const char* name);
    
    void RegisterI2CBusDetector         (I2CBusDetectorFunction     detector);
    void RegisterDeviceDetector         (std::string name, DeviceDetectorFunction     detector);
    void RegisterI2CDeviceDetector      (std::string name, I2CDeviceDetectorFunction  detector);
    void RegisterHIDDeviceDetector      (std::string name,
                                         HIDDeviceDetectorFunction  detector,
                                         uint16_t vid,
                                         uint16_t pid,
                                         int interface  = HID_INTERFACE_ANY,
                                         int usage_page = HID_USAGE_PAGE_ANY,
                                         int usage      = HID_USAGE_ANY);
    
    void RegisterDeviceListChangeCallback(DeviceListChangeCallback, void * receiver);
    void RegisterDetectionProgressCallback(DetectionProgressCallback callback, void* receiver);
    void UnregisterDeviceListChangeCallback(DeviceListChangeCallback, void* receiver);
    void UnregisterDetectionProgressCallback(DetectionProgressCallback callback, void* receiver);

    unsigned int GetDetectionPercent();
    const char*  GetDetectionString();

    std::string                     GetConfigurationDirectory();

    void                            RegisterClient(NetworkClient*);
    size_t                          GetClientCount();
    NetworkClient*                  GetClient(size_t id);
    // Deprecated
    std::vector<NetworkClient*>&    GetClients(); // TODO: hide the vector; do NOT push in here! Use RegisterClient instead
    NetworkServer*                  GetServer();

    ProfileManager*                 GetProfileManager();
    SettingsManager*                GetSettingsManager();

    void DeviceListChanged(int id, ControllerList list, bool removed);
    void DetectionProgressChanged();

    void Cleanup();

    void DetectDevices();

    void StopDeviceDetection();

    void WaitForDeviceDetection();

private:
    void DetectDevicesThreadFunction();

    static std::unique_ptr<ResourceManager>     instance;

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
    std::vector<RGBController*>                 rgb_controllers_hw_sizes;
    std::vector<bool>                           rgb_controllers_hw_sizes_used;
    std::vector<RGBController*>                 rgb_controllers_i2c;
    std::vector<RGBController*>                 rgb_controllers_hid;
    std::vector<RGBController*>                 rgb_controllers_usb;
    std::vector<RGBController*>                 rgb_controllers_misc;
    std::vector<RGBController*>                 rgb_controllers_remote;

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
    std::vector<HIDDeviceDetectorBlock>         hid_device_detectors;
    std::vector<std::string>                    hid_device_detector_strings;

    /*-------------------------------------------------------------------------------------*\
    | Detection Thread and Detection State                                                  |
    \*-------------------------------------------------------------------------------------*/
    std::thread *                               DetectDevicesThread;
    std::mutex                                  DetectDeviceMutex;

    std::atomic<bool>                           detection_is_required;
    std::atomic<unsigned int>                   detection_percent;
    const char*                                 detection_string;
    std::string                                 tested_location;
    
    /*-------------------------------------------------------------------------------------*\
    | Device List Changed Callback                                                          |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DeviceListChangeMutex;
    std::vector<DeviceListChangeBlock>          DeviceListChangeCallbacks;

    /*-------------------------------------------------------------------------------------*\
    | Detection Progress Callback                                                           |
    \*-------------------------------------------------------------------------------------*/
    std::mutex                                  DetectionProgressMutex;
    std::vector<DetectionProgressBlock>         DetectionProgressCallbacks;
};
