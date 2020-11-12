/*-----------------------------------------*\
|  ResourceManager.cpp                      |
|                                           |
|  OpenRGB Resource Manager controls access |
|  to application components including      |
|  RGBControllers, I2C interfaces, and      |
|  network SDK components                   |
|                                           |
|  Adam Honse (CalcProgrammer1) 9/27/2020   |
\*-----------------------------------------*/

#include "ResourceManager.h"
#include "ProfileManager.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
#include <stdlib.h>
#include <string>
#include <hidapi/hidapi.h>

std::unique_ptr<ResourceManager> ResourceManager::instance;

using namespace std::chrono_literals;

ResourceManager *ResourceManager::get()
{
    if(!instance)
    {
        instance = std::make_unique<ResourceManager>();
    }

    return instance.get();
}

ResourceManager::ResourceManager()
{
    /*-------------------------------------------------------------------------*\
    | Initialize Detection Variables                                            |
    \*-------------------------------------------------------------------------*/
    detection_percent     = 100;
    detection_string      = "";
    detection_is_required = false;
    DetectDevicesThread   = nullptr;

    /*-------------------------------------------------------------------------*\
    | Initialize Server Instance                                                |
    \*-------------------------------------------------------------------------*/
    server = new NetworkServer(rgb_controllers_misc);

    /*-------------------------------------------------------------------------*\
    | Load sizes list from file                                                 |
    \*-------------------------------------------------------------------------*/
    profile_manager         = new ProfileManager(rgb_controllers_remote, GetConfigurationDirectory());
    rgb_controllers_hw_sizes   = profile_manager->LoadProfileToList("sizes.ors");

    /*-------------------------------------------------------------------------*\
    | Load settings from file                                                   |
    \*-------------------------------------------------------------------------*/
    settings_manager        = new SettingsManager();
    settings_manager->LoadSettings(GetConfigurationDirectory() + "OpenRGB.json");
}

ResourceManager::~ResourceManager()
{
    Cleanup();
}

void ResourceManager::RegisterI2CBus(i2c_smbus_interface *bus)
{
    busses.push_back(bus);
}

std::vector<i2c_smbus_interface*> & ResourceManager::GetI2CBusses()
{
    return busses;
}

bool ResourceManager::RegisterRGBController(RGBController *rgb_controller, ControllerList list)
{
    if(rgb_controller->name.empty())
    {
        rgb_controller->name = detection_string;
    }
    if(rgb_controller->location.empty())
    {
        rgb_controller->location = tested_location;
    }

    switch(list)
    {
    case CONTROLLER_LIST_I2C:
        rgb_controllers_i2c.push_back(rgb_controller);
        profile_manager->LoadDeviceFromListWithOptions(rgb_controllers_hw_sizes, rgb_controllers_hw_sizes_used, rgb_controller, true, false);
        DeviceListChanged(rgb_controllers_i2c.size() - 1, list, false);
        return true;
    case CONTROLLER_LIST_HID:
        rgb_controllers_hid.push_back(rgb_controller);
        profile_manager->LoadDeviceFromListWithOptions(rgb_controllers_hw_sizes, rgb_controllers_hw_sizes_used, rgb_controller, true, false);
        DeviceListChanged(rgb_controllers_hid.size() - 1, list, false);
        return true;
    case CONTROLLER_LIST_USB:
        rgb_controllers_usb.push_back(rgb_controller);
        profile_manager->LoadDeviceFromListWithOptions(rgb_controllers_hw_sizes, rgb_controllers_hw_sizes_used, rgb_controller, true, false);
        DeviceListChanged(rgb_controllers_usb.size() - 1, list, false);
        return true;
    case CONTROLLER_LIST_MISC:
    case CONTROLLER_LIST_HW:
    case CONTROLLER_LIST_UI:
        rgb_controllers_misc.push_back(rgb_controller);
        profile_manager->LoadDeviceFromListWithOptions(rgb_controllers_hw_sizes, rgb_controllers_hw_sizes_used, rgb_controller, true, false);
        DeviceListChanged(rgb_controllers_misc.size() - 1, CONTROLLER_LIST_MISC, false);
        return true;
    case CONTROLLER_LIST_REMOTE:
        rgb_controllers_remote.push_back(rgb_controller);
        DeviceListChanged(rgb_controllers_remote.size() - 1, list, false);
        return true;
    default:
        return false;
    }
    return false;
}

bool ResourceManager::RemoveRGBController(RGBController* rgb_controller, ControllerList list)
{
    switch(list)
    {
    case CONTROLLER_LIST_I2C:
        for(size_t idx = 0; idx < rgb_controllers_i2c.size(); ++idx)
        {
            if(rgb_controllers_i2c[idx] == rgb_controller)
            {
                rgb_controllers_i2c.erase(rgb_controllers_i2c.begin() + idx);
                DeviceListChanged(idx, list, true);
                return true;
            }
        }
        return false;
    case CONTROLLER_LIST_HID:
        for(size_t idx = 0; idx < rgb_controllers_hid.size(); ++idx)
        {
            if(rgb_controllers_hid[idx] == rgb_controller)
            {
                rgb_controllers_hid.erase(rgb_controllers_hid.begin() + idx);
                DeviceListChanged(idx, list, true);
                return true;
            }
        }
        return false;
    case CONTROLLER_LIST_USB:
        for(size_t idx = 0; idx < rgb_controllers_usb.size(); ++idx)
        {
            if(rgb_controllers_usb[idx] == rgb_controller)
            {
                rgb_controllers_usb.erase(rgb_controllers_usb.begin() + idx);
                DeviceListChanged(idx, list, true);
                return true;
            }
        }
        return false;
    case CONTROLLER_LIST_MISC:
        for(size_t idx = 0; idx < rgb_controllers_misc.size(); ++idx)
        {
            if(rgb_controllers_misc[idx] == rgb_controller)
            {
                rgb_controllers_misc.erase(rgb_controllers_misc.begin() + idx);
                DeviceListChanged(idx, list, true);
                return true;
            }
        }
        return false;
    case CONTROLLER_LIST_REMOTE:
        for(size_t idx = 0; idx < rgb_controllers_remote.size(); ++idx)
        {
            if(rgb_controllers_remote[idx] == rgb_controller)
            {
                rgb_controllers_misc.erase(rgb_controllers_remote.begin() + idx);
                DeviceListChanged(idx, CONTROLLER_LIST_REMOTE, true);
                return true;
            }
        }
        return false;
    case CONTROLLER_LIST_HW:
        // Don't know the list -> try all of them (but only the hardware ones)
        return
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_I2C) |
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_HID) |
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_USB) |
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_MISC);
    default:
        // Don't know the list -> try all of them
        return
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_HW) |
                RemoveRGBController(rgb_controller, CONTROLLER_LIST_REMOTE);
    }
}

size_t ResourceManager::GetControllerCount(ControllerList list) const
{
    switch(list)
    {
    case CONTROLLER_LIST_UI:
        return GetControllerCount(CONTROLLER_LIST_HW) + rgb_controllers_remote.size();
        // Add more lists here if needed
    case CONTROLLER_LIST_HW:
        return rgb_controllers_i2c.size() + rgb_controllers_hid.size() + rgb_controllers_usb.size() + rgb_controllers_misc.size();
    case CONTROLLER_LIST_I2C:
        return rgb_controllers_i2c.size();
    case CONTROLLER_LIST_HID:
        return rgb_controllers_hid.size();
    case CONTROLLER_LIST_USB:
        return rgb_controllers_usb.size();
    case CONTROLLER_LIST_MISC:
        return rgb_controllers_misc.size();
    case CONTROLLER_LIST_REMOTE:
        return rgb_controllers_remote.size();
    }
    return 0;
}

RGBController* ResourceManager::GetController(size_t id, ControllerList list)
{
    switch(list)
    {
    case CONTROLLER_LIST_UI:
        if(id < rgb_controllers_i2c.size())
            return rgb_controllers_i2c[id];
        id -= rgb_controllers_i2c.size();
        if(id < rgb_controllers_hid.size())
            return rgb_controllers_hid[id];
        id -= rgb_controllers_hid.size();
        if(id < rgb_controllers_usb.size())
            return rgb_controllers_usb[id];
        id -= rgb_controllers_usb.size();
        if(id < rgb_controllers_misc.size())
            return rgb_controllers_misc[id];
        id -= rgb_controllers_misc.size();
        if(id < rgb_controllers_remote.size())
            return rgb_controllers_remote[id];
        // Add more lists here if needed
        return nullptr;
    case CONTROLLER_LIST_HW:
        if(id < rgb_controllers_i2c.size())
            return rgb_controllers_i2c[id];
        id -= rgb_controllers_i2c.size();
        if(id < rgb_controllers_hid.size())
            return rgb_controllers_hid[id];
        id -= rgb_controllers_hid.size();
        if(id < rgb_controllers_usb.size())
            return rgb_controllers_usb[id];
        id -= rgb_controllers_usb.size();
        if(id < rgb_controllers_misc.size())
            return rgb_controllers_misc[id];
        // Add more lists here if needed
        return nullptr;
    case CONTROLLER_LIST_I2C:
        if(id < rgb_controllers_i2c.size())
            return rgb_controllers_i2c[id];
        return nullptr;
    case CONTROLLER_LIST_HID:
        if(id < rgb_controllers_hid.size())
            return rgb_controllers_hid[id];
        return nullptr;
    case CONTROLLER_LIST_USB:
        if(id < rgb_controllers_usb.size())
            return rgb_controllers_usb[id];
        return nullptr;
    case CONTROLLER_LIST_MISC:
        if(id < rgb_controllers_misc.size())
            return rgb_controllers_misc[id];
        return nullptr;
    case CONTROLLER_LIST_REMOTE:
        if(id < rgb_controllers_remote.size())
            return rgb_controllers_remote[id];
        return nullptr;
    }
    return nullptr;
}

int ResourceManager::GetUIListIndex(size_t id, ControllerList list) const
{
    switch(list)
    {
    case CONTROLLER_LIST_UI:
    case CONTROLLER_LIST_HW:
    case CONTROLLER_LIST_I2C:
        return id;
    case CONTROLLER_LIST_HID:
        return rgb_controllers_i2c.size() + id;
    case CONTROLLER_LIST_USB:
        return rgb_controllers_i2c.size() + rgb_controllers_hid.size() + id;
    case CONTROLLER_LIST_MISC:
        return rgb_controllers_i2c.size() + rgb_controllers_i2c.size() + rgb_controllers_usb.size() + id;
    case CONTROLLER_LIST_REMOTE:
        id += GetControllerCount(CONTROLLER_LIST_HW);
        return id;
        // Fallthrough
    default:
        break;
    }
    return id;
}


size_t ResourceManager::GetDetectorCount(ControllerList list) const
{
    switch(list)
    {
    case CONTROLLER_LIST_UI:
    case CONTROLLER_LIST_HW:
        return i2c_device_detectors.size() + hid_device_detectors.size() + device_detectors.size();
    case CONTROLLER_LIST_I2C:
        return i2c_device_detectors.size();
    case CONTROLLER_LIST_HID:
        return hid_device_detectors.size();
    case CONTROLLER_LIST_USB:
        return 0;
    case CONTROLLER_LIST_MISC:
        return device_detectors.size();
    default:
        return 0;
    }
}

const char* ResourceManager::GetDetectorName(size_t index, ControllerList list) const
{
    switch(list)
    {
    case CONTROLLER_LIST_UI:
    case CONTROLLER_LIST_HW:
        if(index < i2c_device_detectors.size())
            return i2c_device_detector_strings[index].c_str();
        index -= i2c_device_detectors.size();
        if(index < hid_device_detectors.size())
            return hid_device_detector_strings[index].c_str();
        index -= hid_device_detectors.size();
        if(index < device_detectors.size())
            return device_detector_strings[index].c_str();
        return nullptr;
    case CONTROLLER_LIST_I2C:
        if(index < i2c_device_detectors.size())
            return i2c_device_detector_strings[index].c_str();
        return nullptr;
    case CONTROLLER_LIST_HID:
        if(index < hid_device_detectors.size())
            return hid_device_detector_strings[index].c_str();
        return nullptr;
    case CONTROLLER_LIST_USB:
        return nullptr;
    case CONTROLLER_LIST_MISC:
        if(index < device_detectors.size())
            return device_detector_strings[index].c_str();
        return nullptr;
    default:
        return nullptr;
    }
}

bool ResourceManager::IsDetectorEnabled(size_t index, ControllerList list) const
{
    return IsDetectorEnabled(GetDetectorName(index, list));
}

bool ResourceManager::IsDetectorEnabled(const char* name) const
{
    settings_manager->LockSettings();
    json& detector_settings = settings_manager->GetSettingsRef("Setting_Detectors");
    bool enabled = true;
    json& detectors = detector_settings["detectors"];
    if(detectors.contains(name))
    {
        json& target = detectors[name];
        if(target.is_object())
        {
            if(target.contains("enabled"))
            {
                enabled = target["enabled"];
            }
        }
        else if(target.is_boolean())
        {
            enabled = target;
        }
    }
    settings_manager->UnlockSettings();
    return enabled;
}

bool ResourceManager::SetDetectorEnabled(bool en, size_t index, ControllerList list)
{
    return SetDetectorEnabled(en, GetDetectorName(index, list));
}

bool ResourceManager::SetDetectorEnabled(bool en, const char* name)
{
    settings_manager->LockSettings();
    json& detector_settings = settings_manager->GetSettingsRef("Setting_Detectors");
    json& detectors = detector_settings["detectors"];
    if(detectors.contains(name))
    {
        json& target = detectors[name];
        if(target.is_object())
        {
            target["enabled"] = en;
        }
        else
        {
            target = en;
        }
    }
    else
    {
        detectors[name] = en;
    }
    settings_manager->UnlockSettings();
    return true;
}

void ResourceManager::RegisterI2CBusDetector(I2CBusDetectorFunction detector)
{
    i2c_bus_detectors.push_back(detector);
}

void ResourceManager::RegisterI2CDeviceDetector(std::string name, I2CDeviceDetectorFunction detector)
{
    i2c_device_detector_strings.push_back(name);
    i2c_device_detectors.push_back(detector);
}

void ResourceManager::RegisterDeviceDetector(std::string name, DeviceDetectorFunction detector)
{
    device_detector_strings.push_back(name);
    device_detectors.push_back(detector);
}

void ResourceManager::RegisterHIDDeviceDetector(std::string name,
                               HIDDeviceDetectorFunction  detector,
                               uint16_t vid,
                               uint16_t pid,
                               int interface,
                               int usage_page,
                               int usage)
{
    HIDDeviceDetectorBlock block;

    block.name          = name;
    block.address       = (vid << 16) | pid;
    block.function      = detector;
    block.interface     = interface;
    block.usage_page    = usage_page;
    block.usage         = usage;

    hid_device_detectors.push_back(block);
    hid_device_detector_strings.push_back(name);
}

void ResourceManager::RegisterDeviceListChangeCallback(DeviceListChangeCallback callback, void * receiver)
{
    DeviceListChangeCallbacks.push_back(DeviceListChangeBlock(callback, receiver));
}

void ResourceManager::RegisterDetectionProgressCallback(DetectionProgressCallback new_callback, void * receiver)
{
    DetectionProgressCallbacks.push_back(DetectionProgressBlock(new_callback, receiver));
}

void ResourceManager::UnregisterDeviceListChangeCallback(DeviceListChangeCallback callback, void * arg)
{
    for(size_t idx = 0; idx < DeviceListChangeCallbacks.size(); ++idx)
    {
        if(DeviceListChangeCallbacks[idx].first == callback && DeviceListChangeCallbacks[idx].second == arg)
        {
            DeviceListChangeCallbacks.erase(DeviceListChangeCallbacks.begin());
            break;
        }
    }
}

void ResourceManager::UnregisterDetectionProgressCallback(DetectionProgressCallback callback, void * arg)
{
    for(size_t idx = 0; idx < DetectionProgressCallbacks.size(); ++idx)
    {
        if(DetectionProgressCallbacks[idx].first == callback && DetectionProgressCallbacks[idx].second == arg)
        {
            DetectionProgressCallbacks.erase(DetectionProgressCallbacks.begin());
            break;
        }
    }
}

void ResourceManager::DeviceListChanged(int id, ControllerList list, bool removed)
{
    DeviceListChangeMutex.lock();

    /*-------------------------------------------------*\
    | Device list has changed, call the callbacks       |
    \*-------------------------------------------------*/
    for(unsigned int callback_idx = 0; callback_idx < DeviceListChangeCallbacks.size(); callback_idx++)
    {
        DeviceListChangeCallbacks[callback_idx].first(DeviceListChangeCallbacks[callback_idx].second, id, list, removed);
    }

    /*-------------------------------------------------*\
    | Device list has changed, inform all clients       |
    | connected to this server                          |
    \*-------------------------------------------------*/
    if(server)
    {
        server->DeviceListChanged();
    }

    DeviceListChangeMutex.unlock();
}

void ResourceManager::DetectionProgressChanged()
{
    DetectionProgressMutex.lock();

    /*-------------------------------------------------*\
    | Detection progress has changed, call the callbacks|
    \*-------------------------------------------------*/
    for(unsigned int callback_idx = 0; callback_idx < DetectionProgressCallbacks.size(); callback_idx++)
    {
        DetectionProgressCallbacks[callback_idx].first(DetectionProgressCallbacks[callback_idx].second);
    }

    DetectionProgressMutex.unlock();
}

std::string ResourceManager::GetConfigurationDirectory()
{
    std::string config_dir      = "";
    const char* xdg_config_home = getenv("XDG_CONFIG_HOME");
    const char* home            = getenv("HOME");
    const char* appdata         = getenv("APPDATA");

    /*-----------------------------------------------------*\
    | Check both XDG_CONFIG_HOME and APPDATA environment    |
    | variables.  If neither exist, use current directory   |
    \*-----------------------------------------------------*/
    if(xdg_config_home != NULL)
    {
        config_dir = xdg_config_home;
    }
    else if(appdata != NULL)
    {
        config_dir = appdata;
    }
    else if(home != NULL)
    {
        config_dir = home;
        config_dir = config_dir + "/.config";
    }

    /*-----------------------------------------------------*\
    | If a configuration directory was found, append OpenRGB|
    \*-----------------------------------------------------*/
    if(config_dir != "")
    {
        config_dir = config_dir + "/OpenRGB/";

        /*-------------------------------------------------------------------------*\
        | Create OpenRGB configuration directory if it doesn't exist                |
        \*-------------------------------------------------------------------------*/
        std::experimental::filesystem::create_directories(config_dir);
    }
    else
    {
        config_dir = "./";
    }

    return(config_dir);
}

NetworkServer* ResourceManager::GetServer()
{
    return(server);
}

size_t ResourceManager::GetClientCount()
{
    return clients.size();
}

static void OnNetworkClientDeviceListChange(void*, NetworkClient*, RGBController* controller, bool removed)
{
    if(removed)
    {
        ResourceManager::get()->RemoveRGBController(controller, CONTROLLER_LIST_REMOTE);
    }
    else
    {
        ResourceManager::get()->RegisterRGBController(controller, CONTROLLER_LIST_REMOTE);
    }
}

void ResourceManager::RegisterClient(NetworkClient* client)
{
    client->RegisterDeviceListChangeCallback(OnNetworkClientDeviceListChange, NULL);
    clients.push_back(client);
    // Notify about client list change?
}

NetworkClient* ResourceManager::GetClient(size_t id)
{
    if(id >= clients.size()) return nullptr; // Failsafe
    return clients[id];
}

// Deprecated
std::vector<NetworkClient*>& ResourceManager::GetClients()
{
    return(clients);
}

ProfileManager* ResourceManager::GetProfileManager()
{
    return(profile_manager);
}

SettingsManager* ResourceManager::GetSettingsManager()
{
    return(settings_manager);
}

unsigned int ResourceManager::GetDetectionPercent()
{
    return (detection_percent.load());
}

const char *ResourceManager::GetDetectionString()
{
    return (detection_string);
}

void ResourceManager::Cleanup()
{
    ResourceManager::get()->WaitForDeviceDetection();

    for(int controller_idx = GetControllerCount() - 1; controller_idx >= 0; --controller_idx)
    {
        RemoveRGBController(GetController(controller_idx));
    }

    std::vector<i2c_smbus_interface *> busses_copy = busses;

    busses.clear();

    for(i2c_smbus_interface* bus : busses_copy)
    {
        delete bus;
    }

    if(DetectDevicesThread)
    {
        DetectDevicesThread->join();
        delete DetectDevicesThread;
        DetectDevicesThread = nullptr;
    }
}

void ResourceManager::DetectDevices()
{
    /*-------------------------------------------------*\
    | Do nothing is it is already detecting devices     |
    \*-------------------------------------------------*/
    if(detection_is_required.load())
    {
        return;
    }

    /*-------------------------------------------------*\
    | If there's anything left from the last time,      |
    | we shall remove it first                          |
    \*-------------------------------------------------*/
    detection_percent = 0;
    detection_string  = "";

    DetectionProgressChanged();

    Cleanup();

    /*-------------------------------------------------*\
    | Start the device detection thread                 |
    \*-------------------------------------------------*/
    detection_is_required = true;
    DetectDevicesThread = new std::thread(&ResourceManager::DetectDevicesThreadFunction, this);

    /*-------------------------------------------------*\
    | Release the current thread to allow detection     |
    | thread to start                                   |
    \*-------------------------------------------------*/
    std::this_thread::sleep_for(1ms);
}

void ResourceManager::DetectDevicesThreadFunction()
{
    DetectDeviceMutex.lock();

    hid_device_info*    current_hid_device;
    float               percent             = 0.0f;
    float               percent_denominator = 0.0f;
    unsigned int        hid_device_count    = 0;
    hid_device_info*    hid_devices         = hid_enumerate(0, 0);

    /*-------------------------------------------------*\
    | Calculate the percentage denominator by adding    |
    | the number of I2C and miscellaneous detectors and |
    | the number of enumerated HID devices              |
    |                                                   |
    | Start by iterating through all HID devices in     |
    | list to get a total count                         |
    \*-------------------------------------------------*/
    current_hid_device = hid_devices;

    while(current_hid_device)
    {
        hid_device_count++;

        current_hid_device = current_hid_device->next;
    }

    percent_denominator = i2c_device_detectors.size() + device_detectors.size() + hid_device_count;

    /*-------------------------------------------------*\
    | Start at 0% detection progress                    |
    \*-------------------------------------------------*/
    detection_percent = 0;

    /*-------------------------------------------------*\
    | Detect i2c busses                                 |
    \*-------------------------------------------------*/
    for(unsigned int i2c_bus_detector_idx = 0; i2c_bus_detector_idx < i2c_bus_detectors.size() && detection_is_required.load(); i2c_bus_detector_idx++)
    {
        i2c_bus_detectors[i2c_bus_detector_idx](busses);
    }

    /*-------------------------------------------------*\
    | Detect i2c devices                                |
    \*-------------------------------------------------*/
    for(unsigned int i2c_detector_idx = 0; i2c_detector_idx < i2c_device_detectors.size() && detection_is_required.load(); i2c_detector_idx++)
    {
        detection_string = i2c_device_detector_strings[i2c_detector_idx].c_str();
        // Create a fallback location string
        tested_location = "I2C: ";
        tested_location.append(i2c_device_detector_strings[i2c_detector_idx]);
        DetectionProgressChanged();

        if(IsDetectorEnabled(detection_string))
        {
            try
            {
                std::vector<RGBController*> tempVector; // To be removed
                i2c_device_detectors[i2c_detector_idx](busses, tempVector);
                for(size_t id = 0; id < tempVector.size(); ++id)
                {
                    RegisterRGBController(tempVector[id], CONTROLLER_LIST_I2C);
                }
            }
            catch(std::exception& e)
            {
                printf("An exception caught in I2C device detector for [%s]: %s", detection_string, e.what());
            }
            catch(...)
            {
                printf("An unknown exception caught in I2C device detector for [%s]", detection_string);
                fflush(stdout);
            }
        }

        /*-------------------------------------------------*\
        | Update detection percent                          |
        \*-------------------------------------------------*/
        percent = (i2c_detector_idx + 1.0f) / percent_denominator;

        detection_percent = percent * 100.0f;
    }

    /*-------------------------------------------------*\
    | Detect HID devices                                |
    |                                                   |
    | Reset current device pointer to first device      |
    \*-------------------------------------------------*/
    current_hid_device = hid_devices;

    /*-------------------------------------------------*\
    | Iterate through all devices in list and run       |
    | detectors                                         |
    \*-------------------------------------------------*/
    hid_device_count = 0;

    while(current_hid_device)
    {
        detection_string = "";
        DetectionProgressChanged();
        
        unsigned int addr = (current_hid_device->vendor_id << 16) | current_hid_device->product_id;

        /*-----------------------------------------------------------------------------*\
        | Loop through all available detectors.  If all required information matches,   |
        | run the detector                                                              |
        \*-----------------------------------------------------------------------------*/
        for(unsigned int hid_detector_idx = 0; hid_detector_idx < hid_device_detectors.size() && detection_is_required.load(); hid_detector_idx++)
        {
            if((   hid_device_detectors[hid_detector_idx].address    == addr                                   )
            && ( ( hid_device_detectors[hid_detector_idx].interface  == HID_INTERFACE_ANY                    )
              || ( hid_device_detectors[hid_detector_idx].interface  == current_hid_device->interface_number ) )
#ifdef USE_HID_USAGE
            && ( ( hid_device_detectors[hid_detector_idx].usage_page == HID_USAGE_PAGE_ANY                   )
              || ( hid_device_detectors[hid_detector_idx].usage_page == current_hid_device->usage_page       ) )
            && ( ( hid_device_detectors[hid_detector_idx].usage      == HID_USAGE_ANY                        )
              || ( hid_device_detectors[hid_detector_idx].usage      == current_hid_device->usage            ) )
#endif
            )
            {
                detection_string = hid_device_detectors[hid_detector_idx].name.c_str();

                /*-------------------------------------------------*\
                | Check if this detector is enabled or needs to be  |
                | added to the settings list                        |
                \*-------------------------------------------------*/
                if(IsDetectorEnabled(detection_string))
                {
                    DetectionProgressChanged();
                    try
                    {
                        hid_device_detectors[hid_detector_idx].function(current_hid_device, hid_device_detectors[hid_detector_idx].name);
                    }
                    catch(std::exception& e)
                    {
                        printf("An exception caught in HID device detector for [%s]: %s", detection_string, e.what());
                    }
                    catch(...)
                    {
                        printf("An unknown exception caught in HID device detector for [%s]", detection_string);
                        fflush(stdout);
                    }
                }
            }
        }

        /*-------------------------------------------------*\
        | Update detection percent                          |
        \*-------------------------------------------------*/
        hid_device_count++;

        percent = (i2c_device_detectors.size() + hid_device_count) / percent_denominator;

        detection_percent = percent * 100.0f;

        /*-------------------------------------------------*\
        | Move on to the next HID device                    |
        \*-------------------------------------------------*/
        current_hid_device = current_hid_device->next;
    }

    /*-------------------------------------------------*\
    | Done using the device list, free it               |
    \*-------------------------------------------------*/

    hid_free_enumeration(hid_devices);

    /*-------------------------------------------------*\
    | Detect other devices                              |
    \*-------------------------------------------------*/
    for(unsigned int detector_idx = 0; detector_idx < device_detectors.size() && detection_is_required.load(); detector_idx++)
    {
        detection_string = device_detector_strings[detector_idx].c_str();
        // Create a fallback location string
        tested_location = "MISC: ";
        tested_location.append(device_detector_strings[detector_idx]);
        DetectionProgressChanged();

        if(IsDetectorEnabled(detection_string))
        {
            try
            {
                std::vector<RGBController*> tempVector; // To be removed
                device_detectors[detector_idx](tempVector);
                for(size_t id = 0; id < tempVector.size(); ++id)
                {
                    RegisterRGBController(tempVector[id], CONTROLLER_LIST_MISC);
                }
            }
            catch(std::exception& e)
            {
                printf("An exception caught in MISC device detector for [%s]: %s", detection_string, e.what());
            }
            catch(...)
            {
                printf("An unknown exception caught in MISC device detector for [%s]", detection_string);
                fflush(stdout);
            }
        }

        /*-------------------------------------------------*\
        | Update detection percent                          |
        \*-------------------------------------------------*/
        percent = (i2c_device_detectors.size() + hid_device_count + detector_idx + 1.0f) / percent_denominator;

        detection_percent = percent * 100.0f;
    }

    /*-------------------------------------------------*\
    | Make sure that when the detection is done,        |
    | progress bar is set to 100%                       |
    \*-------------------------------------------------*/

    detection_is_required = false;
    detection_percent = 100;
    detection_string = "";
    tested_location = "";

    DetectionProgressChanged();
    
    DetectDeviceMutex.unlock();

    /*-------------------------------------------------*\
    | If there are controllers not listed in config,    |
    | add them to the list. HID detectors need special  |
    | treatment as they are pre-optimized.              |
    \*-------------------------------------------------*/
    bool save_settings = false;
    settings_manager->LockSettings();
    json& detector_settings = settings_manager->GetSettingsRef("Setting_Detectors");
    json& detectors = detector_settings["detectors"];
    for(size_t idx = 0; idx < i2c_device_detector_strings.size(); ++idx)
    {
        if(!detectors.contains(i2c_device_detector_strings[idx]))
        {
            // TODO: check if this particular detector requires a more sophisticated config
            detectors[i2c_device_detector_strings[idx]] = true;
            save_settings = true;
        }
    }
    for(size_t idx = 0; idx < device_detector_strings.size(); ++idx)
    {
        if(!detectors.contains(device_detector_strings[idx]))
        {
            // TODO: check if this particular detector requires a more sophisticated config
            detectors[device_detector_strings[idx]] = true;
            save_settings = true;
        }
    }
    settings_manager->UnlockSettings();
    if(save_settings)
    {
        settings_manager->SaveSettings();
    }
}

void ResourceManager::StopDeviceDetection()
{
    detection_is_required = false;
    detection_percent = 100;
    detection_string = "Stopping";
}

void ResourceManager::WaitForDeviceDetection()
{
    DetectDeviceMutex.lock();
    DetectDeviceMutex.unlock();
}
