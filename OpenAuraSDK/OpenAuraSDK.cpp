// OpenAuraSDK.cpp : Defines the exported functions for the DLL application.
//

#include "i2c_smbus.h"
#include "i2c_smbus_piix4.h"
#include "i2c_smbus_i801.h"
#include "AuraController.h"
#include "OpenAuraSDK.h"
#include "smbus_detect.h"

#include <vector>
#include <string>

std::vector<AuraController *> controllers;

void CreateAuraDevice(AuraBusDriverType driver, unsigned int base, aura_dev_id dev)
{
    i2c_smbus_interface * bus;
    AuraController * aura;

    switch (driver)
    {
    case I2C_DRIVER_SMBUS_PIIX4:
        bus = new i2c_smbus_piix4();
        ((i2c_smbus_piix4 *)bus)->piix4_smba = base;
        break;

    case I2C_DRIVER_SMBUS_I801:
        bus = new i2c_smbus_i801();
        ((i2c_smbus_i801 *)bus)->i801_smba = base;
        break;

    default:
        return;
    }

    aura = new AuraController();
    aura->bus = bus;
    aura->dev = dev;

    controllers.push_back(aura);
}

//code adapted from i2c-tools i2cdetect.c
#define MODE_AUTO   0
#define MODE_QUICK  1
#define MODE_READ   2
#define MODE_FUNC   3

void DetectI2C(i2c_smbus_interface * bus, int mode, const char* filename="i2cdetect.txt")
{
    int i, j;
    int res;
    int slave_addr;

    freopen(filename, "w", stdout);

    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");

    for (i = 0; i < 128; i += 16)
    {
        printf("%02x: ", i);

        for (j = 0; j < 16; j++)
        {
            /* Set slave address */
            slave_addr = i + j;

			if (slave_addr == 0x08)
			{
				int a = 0;
			}
            /* Probe this address */
            switch (mode)
            {
            case MODE_QUICK:
                res = bus->i2c_smbus_write_quick(slave_addr, I2C_SMBUS_WRITE);
                break;
            case MODE_READ:
                res = bus->i2c_smbus_read_byte(slave_addr);
                break;
            default:
                if ((i + j >= 0x30 && i + j <= 0x37)
                 || (i + j >= 0x50 && i + j <= 0x5F))
                    res = bus->i2c_smbus_read_byte(slave_addr);
                else
                    res = bus->i2c_smbus_write_quick(slave_addr, I2C_SMBUS_WRITE);
                break;
            }

            if (res < 0)
            {
                printf("-- ");
            }
            else
            {
                printf("%02x ", i + j);
            }
        }
        printf("\r\n");
    }
}

void DumpAuraDevices(const char* filename="aura_dump.txt")
{
    freopen(filename, "w", stdout);
    for (unsigned int i = 0; i < controllers.size(); i++)
    {
		uint8_t d_colors[15] = {0};
		uint8_t e_colors[15] = {0};
		for(auto d=0 ; d < 15 ; d++) {
			d_colors[d] = controllers[i]->AuraRegisterRead(AURA_REG_COLORS_DIRECT + d);
			e_colors[d] = controllers[i]->AuraRegisterRead(AURA_REG_COLORS_EFFECT + d);
		}


        unsigned char direct  = controllers[i]->AuraRegisterRead( AURA_REG_DIRECT);
        unsigned char mode    = controllers[i]->AuraRegisterRead( AURA_REG_MODE);

        printf("Controller %d\r\n", i);
        printf("Direct Mode:    %d \r\n", direct  );
        printf("Mode Value:     %d \r\n", mode    );

		for (auto d = 0; d < 15; d++) {
			printf("Direct Color %X : %d \r\n", d, d_colors[d]);
		}

		printf("----------------------------------------------\n");

		for (auto d = 0; d < 15; d++) {
			printf("Effect Color %X : %d \r\n", d, e_colors[d]);
		}
        
    }
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow)
{
    //CreateAuraDevice(I2C_DRIVER_SMBUS_PIIX4, 0x0B00, 0x77);
    //CreateAuraDevice(I2C_DRIVER_SMBUS_PIIX4, 0x0B00, 0x71);
    //CreateAuraDevice(I2C_DRIVER_SMBUS_PIIX4, 0x0B00, 0x73);
    //CreateAuraDevice(I2C_DRIVER_SMBUS_PIIX4, 0x0B00, 0x74);
    //CreateAuraDevice(I2C_DRIVER_SMBUS_PIIX4, 0x0B20, 0x4E);
	auto bus = DetectSMBUHandle();
	if (!detectSMBUS(bus)) {
		printf("%s\n", bus.name);
		printf("PCI BUS:%4d | Device ID:%4d | Function:%4d |", bus.pci_bus, bus.deviceID, bus.function);
	}
	else {
		printf("There was an error detecting SMBUS");
		return TRUE;
	}
    CreateAuraDevice(I2C_DRIVER_SMBUS_I801, 0x3000, bus.pci_bus);

    DumpAuraDevices();

    DetectI2C(controllers[0]->bus, MODE_AUTO);

    //for (unsigned int i = 0; i < controllers.size(); i++)
    //{
    //    controllers[i]->AuraRegisterWrite(AURA_REG_COLORS_EFFECT, 255);
    //    controllers[i]->AuraRegisterWrite(AURA_REG_DIRECT, 0);
    //    controllers[i]->AuraRegisterWrite(AURA_REG_MODE, AURA_MODE_RAINBOW);
    //    controllers[i]->AuraRegisterWrite(AURA_REG_APPLY, AURA_APPLY_VAL);
    //}

    return TRUE;
}