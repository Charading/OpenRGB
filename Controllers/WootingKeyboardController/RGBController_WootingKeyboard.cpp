/*-----------------------------------------*\
|  RGBController_WootingKeyboard.cpp          |
|                                           |
|  Generic RGB Interface for Wooting RGB      |
|  keyboard devices                         |
|                                           |
|  Adam Honse (CalcProgrammer1) 7/4/2020    |
\*-----------------------------------------*/

#include "RGBController_WootingKeyboard.h"

//0xFFFFFFFF indicates an unused entry in matrix
#define NA 0xFFFFFFFF
#define RGB_RAW_BUFFER_SIZE 96

static unsigned int matrix_map[6][17] =
{
    {0, NA, 12, 18, 24, 30, 36, 42, 48, 54, 60, 66, 72, 78, 84, 90, 96},
    {1, 7, 13, 19, 25, 31, 37, 43, 49, 55, 61, 67, 73, 79, 85, 91, 97},
    {2, 8, 14, 20, 26, 32, 38, 44, 50, 56, 62, 68, 74, 80, 86, 92, 98},
    {3, 9, 15, 21, 27, 33, 39, 45, 51, 57, 63, 69, 75, 81, NA, NA, NA},
    {4, 10, 16, 22, 28, 34, 40, 46, 52, 58, 64, 70, NA, 82, NA, 94, NA},
    {5, 11, 17, NA, NA, NA, 41, NA, NA, NA, 65, 71, 77, 83, 89, 95, 101}
};

const unsigned int rgb_led_index[6][17] =
{
    {0, NA, 11, 12, 23, 24, 36, 47, 85, 84, 49, 48, 59, 61, 73, 81, 80},
    {2, 1, 14, 13, 26, 25, 35, 38, 37, 87, 86, 95, 51, 63, 75, 72, 74},
    {3, 4, 15, 16, 27, 28, 39, 42, 40, 88, 89, 52, 53, 71, 76, 83, 77},
    {5, 6, 17, 18, 29, 30, 41, 46, 44, 90, 93, 54, 57, 65, NA, NA, NA},
    {9, 8, 19, 20, 31, 34, 32, 45, 43, 91, 92, 55, NA, 66, NA, 78, NA},
    {10, 22, 21, NA, NA, NA, 33, NA, NA, NA, 94, 58, 67, 68, 70, 79, 82}
};

static const char *zone_names[] =
    {
        "Keyboard"};

static zone_type zone_types[] =
    {
        ZONE_TYPE_MATRIX,
};

static const unsigned int zone_sizes[] =
    {
        102};

static const char *led_names[] =
    {
        "Key: Escape",
        "Key: `",
        "Key: Tab",
        "Key: Caps Lock",
        "Key: Left Shift",
        "Key: Left Control",
        "Unused",
        "Key: 1",
        "Key: Q",
        "Key: A",
        "Key: \\ (ISO)", //iso key - 10
        "Key: Left Windows",
        "Key: F1",
        "Key: 2",
        "Key: W",
        "Key: S",
        "Key: Z",
        "Key: Left Alt",
        "Key: F2",
        "Key: 3",
        "Key: E", //20
        "Key: D",
        "Key: X",
        "Unused", //space
        "Key: F3",
        "Key: 4",
        "Key: R",
        "Key: F",
        "Key: C",
        "Unused",  //space
        "Key: F4", //30
        "Key: 5",
        "Key: T",
        "Key: G",
        "Key: V",
        "Unused", //space
        "Key: F5",
        "Key: 6",
        "Key: Y",
        "Key: H",
        "Key: B", //40
        "Key: Space",
        "Key: F6",
        "Key: 7",
        "Key: U",
        "Key: J",
        "Key: N",
        "Unused", //space
        "Key: F7",
        "Key: 8",
        "Key: I",
        "Key: K",
        "Key: M",
        "Unused", //space
        "Key: F8",
        "Key: 9",
        "Key: O",
        "Key: L",
        "Key: ,",
        "Unused", //space
        "Key: F9",
        "Key: 0",
        "Key: P",
        "Key: ;",
        "Key: .",
        "Key: Right Alt",
        "Key: F10",
        "Key: -",
        "Key: [",
        "Key: '",
        "Key: /",
        "Key: Right Windows",
        "Key: F11",
        "Key: =",
        "Key: ]",
        "Key: #", //iso only
        "Unused",
        "Key: Right Fn",
        "Key: F12",
        "Key: Backspace",
        "Key: \\ (ANSI)",
        "Key: Enter",
        "Key: Right Shift",
        "Key: Right Control",
        "Key: Print Screen",
        "Key: Insert",
        "Key: Delete",
        "Unused",
        "Unused",
        "Key: Left Arrow",
        "Key: Scroll Lock",
        "Key: Pause/Break",
        "Key: Home",
        "Key: End",
        "Unused",
        "Key: Up Arrow",
        "Key: Down Arrow",
        "Key: Mode",
        "Key: Page Up",
        "Key: Page Down",
        "Unused",
        "Unused",
        "Key: Right Arrow"};

RGBController_WootingKeyboard::RGBController_WootingKeyboard(WootingKeyboardController *Wooting_ptr)
{
    Wooting = Wooting_ptr;

    name = "Wooting One";
    type = DEVICE_TYPE_KEYBOARD;
    description = "Wooting Keyboard Device";

    mode Direct;
    Direct.name = "Direct";
    Direct.value = 0xFFFF;
    Direct.flags = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_WootingKeyboard::~RGBController_WootingKeyboard()
{
    /*---------------------------------------------------------*\
    | Delete the matrix map                                     |
    \*---------------------------------------------------------*/
    for (unsigned int zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        if (zones[zone_index].matrix_map != NULL)
        {
            delete zones[zone_index].matrix_map;
        }
    }
}

void RGBController_WootingKeyboard::SetupZones()
{
    /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    unsigned int total_led_count = 0;
    for (unsigned int zone_idx = 0; zone_idx < 1; zone_idx++)
    {
        zone new_zone;
        new_zone.name = zone_names[zone_idx];
        new_zone.type = zone_types[zone_idx];
        new_zone.leds_min = zone_sizes[zone_idx];
        new_zone.leds_max = zone_sizes[zone_idx];
        new_zone.leds_count = zone_sizes[zone_idx];
        new_zone.matrix_map = new matrix_map_type;
        new_zone.matrix_map->height = 6;
        new_zone.matrix_map->width = 17;
        new_zone.matrix_map->map = (unsigned int *)&matrix_map;
        zones.push_back(new_zone);

        total_led_count += zone_sizes[zone_idx];
    }

    for (unsigned int led_idx = 0; led_idx < total_led_count; led_idx++)
    {
        led new_led;
        new_led.name = led_names[led_idx];
        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_WootingKeyboard::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_WootingKeyboard::DeviceUpdateLEDs()
{
    const uint8_t pwm_mem_map[48] =
    {
        0x0,   0x1,  0x2,  0x3,  0x4,  0x5,  0x8,  0x9,  0xa,  0xb,  0xc,  0xd,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d
    };

    unsigned char buffer0[RGB_RAW_BUFFER_SIZE] = {0};
    unsigned char buffer1[RGB_RAW_BUFFER_SIZE] = {0};
    unsigned char buffer2[RGB_RAW_BUFFER_SIZE] = {0};
    unsigned char buffer3[RGB_RAW_BUFFER_SIZE] = {0};

    for(std::size_t color_idx = 0; color_idx < colors.size(); color_idx++)
    {
        unsigned char led_index = rgb_led_index[color_idx % 6][color_idx / 6];
        if(led_index > 95)
            continue;

        unsigned char *buffer_pointer = buffer0;

        if (led_index >= 72)
            buffer_pointer = buffer3;
        else if (led_index >= 48)
            buffer_pointer = buffer2;
        else if (led_index >= 24)
            buffer_pointer = buffer1;
        else
            buffer_pointer = buffer0;

        unsigned char buffer_index = pwm_mem_map[led_index % 24];
        buffer_pointer[buffer_index + 0x00] = RGBGetRValue(colors[color_idx]);
        buffer_pointer[buffer_index + 0x10] = RGBGetGValue(colors[color_idx]);
        buffer_pointer[buffer_index + 0x20] = RGBGetBValue(colors[color_idx]);
    }

    Wooting->SendColors(buffer0, buffer1, buffer2, buffer3);
}

void RGBController_WootingKeyboard::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_WootingKeyboard::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_WootingKeyboard::SetCustomMode()
{
}

void RGBController_WootingKeyboard::DeviceUpdateMode()
{
}
