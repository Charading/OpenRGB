/*-----------------------------------------*\
|  RGBController_AsusAuraKeyboard.h         |
|                                           |
|  Generic RGB Interface for Asus Aura      |
|  USB controller driver                    |
|                                           |
|  Adam Honse (CalcProgrammer1) 3/19/2021   |
\*-----------------------------------------*/

#pragma once
#include "RGBController.h"
#include "AsusAuraKeyboardController.h"

enum AsusKbMappingLayoutType
{
    DEFAULT_LAYOUT,
    SCOPE_LAYOUT,
};

typedef struct
{
    const char*  name;
    unsigned char idx;
} led_type;

typedef struct {
    const char* name;
    zone_type type;
    const unsigned int size;
}led_zone;

class RGBController_AuraKeyboard : public RGBController
{
public:
    RGBController_AuraKeyboard(AuraKeyboardController* aura_ptr, AsusKbMappingLayoutType kb_layout);
    ~RGBController_AuraKeyboard();

    void        SetupZones();

    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        SetCustomMode();
    void        DeviceUpdateMode();

private:
    AuraKeyboardController* aura;

    std::vector<led_type> led_names;

    std::vector<led_zone> led_zones =
    {
        {"Keyboard", ZONE_TYPE_MATRIX, 106},
    };

    unsigned int total_led_count = 0;
};
