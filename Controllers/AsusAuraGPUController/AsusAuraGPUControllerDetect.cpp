/*-----------------------------------------*\
|  AsusAuraGPUControllerDetect.cpp          |
|                                           |
|  Driver for ASUS Aura RGB on GPUs         |
|                                           |
|  Jan Rettig (Klapstuhl) 14.02.2020        |
\*-----------------------------------------*/

#include "LogManager.h"
#include "Detector.h"
#include "AsusAuraGPUController.h"
#include "RGBController.h"
#include "RGBController_AsusAuraGPU.h"
#include "i2c_smbus.h"
#include "pci_ids.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std::chrono_literals;

typedef struct
{
    int             pci_vendor;
    int             pci_device;
    int             pci_subsystem_vendor;
    int             pci_subsystem_device;
    unsigned char   controller_address;
    const char *    name;
} gpu_pci_device;

#define GPU_NUM_DEVICES (sizeof(device_list) / sizeof(device_list[ 0 ]))

static const gpu_pci_device device_list[] =
{
    { NVIDIA_VEN,       NVIDIA_GTX1060_DEV,     ASUS_SUB_VEN,       ASUS_GTX1060_STRIX,       0x29,     "ASUS GTX 1060 Strix"       },
    { AMD_GPU_VEN,      AMD_VEGA10_DEV,         ASUS_SUB_VEN,       ASUS_VEGA64_STRIX,        0x29,     "ASUS Vega 64 Strix"        },
    { AMD_GPU_VEN,      AMD_NAVI10_DEV,        ASUS_SUB_VEN,       ASUS_RX5700XT_STRIX_GAMING_OC,         0x2A,     "ASUS RX 5700XT Strix Gaming OC"         }
};

/******************************************************************************************\
*                                                                                          *
*   TestForAuraGPUController                                                               *
*                                                                                          *
*       Tests the given address to see if an Aura GPU controller exists there.             *
*                                                                                          *
\******************************************************************************************/

bool TestForAsusAuraGPUController(i2c_smbus_interface* bus, unsigned char address)
{
    bool pass = false;

    unsigned char aura_gpu_magic_high = bus->i2c_smbus_read_byte_data(address, 0x20);                        // High Byte of magic (0x15)                        
    unsigned char aura_gpu_magic_low = bus->i2c_smbus_read_byte_data(address, 0x21);                         // Low Byte of magic (0x89)

    if((aura_gpu_magic_high << 8) + aura_gpu_magic_low == AURA_GPU_MAGIC_VAL)
    {
        pass = true;
    }

    return(pass);

}   /* TestForAuraGPUController() */

/******************************************************************************************\
*                                                                                          *
*   DetectAuraGPUControllers                                                               *
*                                                                                          *
*       Detect Aura GPU controllers on the enumerated I2C busses.                          *
*                                                                                          *
\******************************************************************************************/

void DetectAsusAuraGPUControllers(std::vector<i2c_smbus_interface*> &busses)
{
    AuraGPUController* new_aura_gpu;
    RGBController_AuraGPU* new_controller;

    for(unsigned int dev_list_id = 0; dev_list_id < GPU_NUM_DEVICES; dev_list_id++)
    {
        LOG_DEBUG("DEVICE LIST ENTRY:");
        LOG_DEBUG("PCI DEVICE NAME:", device_list[dev_list_id].name);
        LOG_DEBUG("PCI VENDOR: 0x%04x", device_list[dev_list_id].pci_vendor);
        LOG_DEBUG("PCI DEVICE: 0x%04x", device_list[dev_list_id].pci_device);
        LOG_DEBUG("PCI SUBSYSTEM VENDOR: 0x%04x", device_list[dev_list_id].pci_subsystem_vendor);
        LOG_DEBUG("PCI SUBSYSTEM DEVICE: 0x%04x", device_list[dev_list_id].pci_subsystem_device);
    }
    for (unsigned int bus = 0; bus < busses.size(); bus++)
    {
        LOG_DEBUG("NEW PCI DEVICE DETECTED!");
        LOG_DEBUG("PCI VENDOR: 0x%04x", busses[bus]->pci_vendor);
        LOG_DEBUG("PCI DEVICE: 0x%04x", busses[bus]->pci_device);
        LOG_DEBUG("PCI SUBSYSTEM VENDOR: 0x%04x", busses[bus]->pci_subsystem_vendor);
        LOG_DEBUG("PCI SUBSYSTEM DEVICE: 0x%04x", busses[bus]->pci_subsystem_device);
        for(unsigned int dev_idx = 0; dev_idx < GPU_NUM_DEVICES; dev_idx++)
        {
            if(busses[bus]->pci_vendor           == device_list[dev_idx].pci_vendor           &&
               busses[bus]->pci_device           == device_list[dev_idx].pci_device           &&
               busses[bus]->pci_subsystem_vendor == device_list[dev_idx].pci_subsystem_vendor &&
               busses[bus]->pci_subsystem_device == device_list[dev_idx].pci_subsystem_device)
            {
                LOG_DEBUG("PCI DEVICE IN DEVICE LIST!");
                LOG_DEBUG("PCI VENDOR: 0x%04x", busses[bus]->pci_vendor);
                LOG_DEBUG("PCI DEVICE: 0x%04x", busses[bus]->pci_device);
                LOG_DEBUG("PCI SUBSYSTEM VENDOR: 0x%04x", busses[bus]->pci_subsystem_vendor);
                LOG_DEBUG("PCI SUBSYSTEM DEVICE: 0x%04x", busses[bus]->pci_subsystem_device);
                if (TestForAsusAuraGPUController(busses[bus], device_list[dev_idx].controller_address))
                {
                    LOG_DEBUG("NEW ASUS AURA GPU DETECTED!");
                    new_aura_gpu         = new AuraGPUController(busses[bus], device_list[dev_idx].controller_address);
                    new_controller       = new RGBController_AuraGPU(new_aura_gpu);
                    new_controller->name = device_list[dev_idx].name;
                    LOG_DEBUG("GPU NAME: %c", device_list[dev_idx].name);
                    ResourceManager::get()->RegisterRGBController(new_controller);
                }
            }
        }
    }
} /* DetectAsusAuraGPUControllers() */

REGISTER_I2C_DETECTOR("ASUS Aura GPU", DetectAsusAuraGPUControllers);
