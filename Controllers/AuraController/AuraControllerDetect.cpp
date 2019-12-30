#include "AuraController.h"
#include "RGBController.h"
#include "RGBController_Aura.h"
#include "i2c_smbus.h"
#include <algorithm>
#include <chrono>
#include <set>
#include <thread>
#include <vector>

/*----------------------------------------------------------------------*\
| This list contains the available SMBus addresses for mapping Aura RAM  |
\*----------------------------------------------------------------------*/

std::set<unsigned char> const static aura_ram_addresses {
    0x70,
    0x71,
    0x73,
    0x74,
    0x75,
    0x76,
    0x78,
    0x79,
    0x7A,
    0x7B,
    0x7C,
    0x7D,
    0x7E,
    0x7F,
    0x4F,
    0x66,
    0x67
};

std::set<unsigned char> const static aura_regular_addresses {
    0x40,
    0x4E,
    0x4F,
    0x66
};

/******************************************************************************************\
*                                                                                          *
*   TestForAuraController                                                                  *
*                                                                                          *
*       Tests the given address to see if an Aura controller exists there.  First does a   *
*       quick write to test for a response, and if so does a simple read at 0xA0 to test   *
*       for incrementing values 0...F which was observed at this location during data dump *
*                                                                                          *
\******************************************************************************************/

bool TestForAuraController(i2c_smbus_interface* bus, unsigned char address)
{
    bool pass = false;

    int res = bus->i2c_smbus_write_quick(address, I2C_SMBUS_WRITE);

    if (res >= 0)
    {
        pass = true;

        for (int i = 0xA0; i < 0xB0; i++)
        {
            res = bus->i2c_smbus_read_byte_data(address, i);

            if (res != (i - 0xA0))
            {
                pass = false;
            }
        }
    }

    return(pass);

}   /* TestForAuraController() */

/******************************************************************************************\
*                                                                                          *
*   DetectAuraControllers                                                                  *
*                                                                                          *
*       Detect Aura controllers on the enumerated I2C busses.  Searches for Aura-enabled   *
*       RAM at 0x77 and tries to initialize their slot addresses, then searches for them   *
*       at their correct initialized addresses.  Also looks for motherboard controller at  *
*       address 0x4E.                                                                      *
*                                                                                          *
*           bus - pointer to i2c_smbus_interface where Aura device is connected            *
*           dev - I2C address of Aura device                                               *
*                                                                                          *
\******************************************************************************************/

void DetectAuraControllers(std::vector<i2c_smbus_interface*> &busses, std::vector<RGBController*> &rgb_controllers)
{
    // Prepare list of all unique addresses
    std::vector<unsigned char> addresses;
    std::set_union(aura_ram_addresses.begin(), aura_ram_addresses.end(),
                   aura_regular_addresses.begin(), aura_regular_addresses.end(),
                   std::back_inserter(addresses));

    for (auto bus : busses)
    {
        // Remap Aura-enabled RAM modules on 0x77
        for (unsigned int slot = 0; slot < 8; slot++)
        {
            int res = bus->i2c_smbus_write_quick(0x77, I2C_SMBUS_WRITE);

            if (res < 0)
            {
                break;
            }

            AuraController temp_controller(bus, 0x77);

            // Search through available addresses and skip over ones that are already in use
            for (auto addr : aura_ram_addresses) {
                if (bus->i2c_smbus_write_quick(addr, I2C_SMBUS_WRITE) < 0)
                    continue;
                temp_controller.AuraRegisterWrite(AURA_REG_SLOT_INDEX, slot);
                temp_controller.AuraRegisterWrite(AURA_REG_I2C_ADDRESS, (addr << 1));
            }
        }

        // Add all controllers found on bus
        for (auto addr : addresses)
        {
            if (TestForAuraController(bus, addr))
            {
                auto new_aura = new AuraController(bus, addr);
                auto new_controller = new RGBController_Aura(new_aura);
                rgb_controllers.push_back(new_controller);
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

}   /* DetectAuraControllers() */
