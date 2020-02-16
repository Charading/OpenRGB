/************************************************\
*            AuraGPUControllerDetect.cpp         *
*         Driver for ASUS Aura RGB on GPUs       *
*                                                *
*        Jan Rettig (Klapstuhl) 14.02.2020       *
\************************************************/

#include "AuraGPUController.h"
#include "RGBController.h"
#include "RGBController_AuraGPU.h"
#include "i2c_smbus.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>

static void Sleep(unsigned int milliseconds)
{
    usleep(1000 * milliseconds);
}
#endif

/*-------------------------------------------------------------*\
| This list contains the available I2C addresses for Aura GPUs  |
\*-------------------------------------------------------------*/
#define AURA_GPU_ADDRESS_COUNT 3

static const unsigned char aura_gpu_addresses[] =
{
    0x29,
    0x2A,
    0x60
};


bool TestForAuraGPUController(i2c_smbus_interface* bus, unsigned char address)
{
    bool pass = false;

    unsigned char aura_gpu_magic_high = bus->i2c_smbus_read_byte_data(address, 0x20);                        // High Byte of magic (0x15)                        
    unsigned char aura_gpu_magic_low = bus->i2c_smbus_read_byte_data(address, 0x21);                         // Low Byte of magic (0x89)

    if((aura_gpu_magic_high << 8) + aura_gpu_magic_low == AURA_GPU_MAGIC_VAL) {
        pass = true;
    }

    return(pass);

}   /* TestForAuraGPUController() */


void DetectAuraGPUControllers(std::vector<i2c_smbus_interface*> &busses, std::vector<RGBController*> &rgb_controllers)
{
    AuraGPUController* new_aura_gpu;
    RGBController_AuraGPU* new_controller;

    for (unsigned int bus = 0; bus < busses.size(); bus++)
    {
        // Add Aura-enabled GPU controllers
        for (unsigned int address_list_idx = 0; address_list_idx < AURA_GPU_ADDRESS_COUNT; address_list_idx++)
        {
            if (TestForAuraGPUController(busses[bus], aura_gpu_addresses[address_list_idx]))
            {
                new_aura_gpu = new AuraGPUController(busses[bus], aura_gpu_addresses[address_list_idx]);
                new_controller = new RGBController_AuraGPU(new_aura_gpu);
                rgb_controllers.push_back(new_controller);
            }

            Sleep(1);
        }
    }
} /* DetectAuraGPUControllers() */