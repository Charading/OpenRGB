#include "DeviceDetector.h"
#include "ResourceManager.h"

DeviceDetector::DeviceDetector(std::string name, DeviceDetectorFunction detector)
{
    ResourceManager::get()->RegisterDeviceDetector(name, detector);
}

I2CDeviceDetector::I2CDeviceDetector(std::string name, I2CDeviceDetectorFunction detector)
{
    ResourceManager::get()->RegisterI2CDeviceDetector(name, detector);
}

I2CPCIDeviceDetector::I2CPCIDeviceDetector(std::string name, I2CPCIDeviceDetectorFunction detector, uint16_t ven_id, uint16_t dev_id, uint16_t subven_id, uint16_t subdev_id, uint8_t i2c_addr)
{
    ResourceManager::get()->RegisterI2CPCIDeviceDetector(name, detector, ven_id, dev_id, subven_id, subdev_id, i2c_addr);
}

I2CBusDetector::I2CBusDetector(I2CBusDetectorFunction detector)
{
    ResourceManager::get()->RegisterI2CBusDetector(detector);
}

HIDDeviceDetector::HIDDeviceDetector(std::string name, HIDDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage)
{
    ResourceManager::get()->RegisterHIDDeviceDetector(name, detector, vid, pid, interface, usage_page, usage);
}

HIDWrappedDeviceDetector::HIDWrappedDeviceDetector(std::string name, HIDWrappedDeviceDetectorFunction detector, uint16_t vid, uint16_t pid, int interface, int usage_page, int usage)
{
    ResourceManager::get()->RegisterHIDWrappedDeviceDetector(name, detector, vid, pid, interface, usage_page, usage);
}

DynamicDetector::DynamicDetector(std::string name, DynamicDetectorFunction detector)
{
    ResourceManager::get()->RegisterDynamicDetector(name, detector);
}

PreDetectionHook::PreDetectionHook(PreDetectionHookFunction hook)
{
    ResourceManager::get()->RegisterPreDetectionHook(hook);
}
