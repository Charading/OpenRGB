/*-----------------------------------------*\
|  RGBController_HyperXAlloyElite2.cpp      |
|                                           |
|  Generic RGB Interface for HyperX Alloy   |
|  Elite2 RGB Keyboard                      |
|                                           |
|  KundaPanda (vojdo) 02/04/2021            |
\*-----------------------------------------*/

#include "RGBController_HyperXAlloyElite2.h"

using namespace std::chrono_literals;

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

static unsigned int matrix_map[8][22] =
{
    {  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA,  NA, 102, 105, 106,  96,  NA,  NA },
    { 108, 109, 110, 111, 112,  NA, 113, 114, 115,  NA, 116, 117, 118,  NA, 119, 120, 121,  NA, 122, 123, 124, 125 },
    {   0,  11,  17,  22,  27,  NA,  33,  38,  43,  49,  55,  61,  65,  68,  NA,  74,  78,  83,  NA,  NA,  NA,  NA },
    {   1,   6,  12,  18,  23,  28,  34,  39,  44,  50,  56,  62,  66,  69,  NA,  75,  79,  84,  87,  92,  97, 103 },
    {   2,  NA,   7,  13,  19,  24,  29,  35,  40,  45,  51,  57,  63,  67,  70,  76,  80,  85,  88,  93,  98, 104 },
    {   3,  NA,   8,  14,  20,  25,  30,  36,  41,  46,  52,  58,  64,  NA,  71,  NA,  NA,  NA,  89,  94,  99,  NA },
    {   4,  NA,  NA,   9,  15,  21,  26,  31,  37,  42,  47,  53,  59,  72,  NA,  NA,  81,  NA,  90,  95, 100, 107 },
    {   5,  10,  16,  NA,  NA,  NA,  NA,  32,  NA,  NA,  NA,  48,  60,  54,  73,  77,  82,  86,  91,  NA, 101,  NA }
};

static const char* zone_names[] =
{
    "Keyboard",
};

static zone_type zone_types[] =
{
    ZONE_TYPE_MATRIX,
};

static const unsigned int zone_sizes[] =
{
    126,
};

static const char *led_names[] =
{
    "Key: Escape",
    "Key: `",
    "Key: Tab",
    "Key: Caps Lock",
    "Key: Left Shift",
    "Key: Left Control",
    // Skip index 6
    "Key: 1",
    "Key: Q",
    "Key: A",
    "Key: Z",
    "Key: Left Windows",
    "Key: F1",
    "Key: 2",
    "Key: W",
    "Key: S",
    "Key: X",
    "Key: Left Alt",
    "Key: F2",
    "Key: 3",
    "Key: E",
    "Key: D",
    "Key: C",
    // Skip index 23
    "Key: F3",
    "Key: 4",
    "Key: R",
    "Key: F",
    "Key: V",
    // Skip index 29
    "Key: F4",
    "Key: 5",
    "Key: T",
    "Key: G",
    "Key: B",
    "Key: Space",
    "Key: F5",
    "Key: 6",
    "Key: Y",
    "Key: H",
    "Key: N",
    // Skip index 41
    "Key: F6",
    "Key: 7",
    "Key: U",
    "Key: J",
    "Key: M",
    // Skip index 47
    "Key: F7",
    "Key: 8",
    "Key: I",
    "Key: K",
    "Key: ,",
    "Key: Right Alt",
    "Key: F8",
    "Key: 9",
    "Key: O",
    "Key: L",
    "Key: .",
    // Skip index 59
    "Key: Menu",
    "Key: F9",
    "Key: 0",
    "Key: P",
    "Key: ;",
    "Key: /",
    "Key: Right Windows",
    "Key: F10",
    "Key: -",
    "Key: [",
    // Skip index 70
    // Skip index 71
    "Key: '",
    "Key: F11",
    "Key: =",
    // Skip index 75
    // Skip index 76
    "Key: ]",
    "Key: F12",
    "Key: Backspace",
    "Key: \\ (ANSI)",
    "Key: Enter (ISO)",
    "Key: Right Shift",
    "Key: Right Control",
    "Key: Print Screen",
    "Key: Insert",
    "Key: Delete",
    // Skip index 87
    // Skip index 88
    "Key: Left Arrow",
    "Key: Scroll Lock",
    "Key: Home",
    "Key: End",
    // Skip index 93
    "Key: Up Arrow",
    "Key: Down Arrow",
    "Key: Pause/Break",
    "Key: Page Up",
    "Key: Page Down",
    // Skip index 99
    // Skip index 100
    "Key: Right Arrow",
    // Skip index 102
    "Key: Num Lock",
    "Key: Number Pad 7",
    "Key: Number Pad 4",
    "Key: Number Pad 1",
    "Key: Number Pad 0",
    // Skip index 108
    "Key: Number Pad /",
    "Key: Number Pad 8",
    "Key: Number Pad 5",
    "Key: Number Pad 2",
    // Skip index 113
    // Skip index 114
    // Last multimedia key
    "Key: Media Mute",
    "Key: Number Pad *",
    "Key: Number Pad 9",
    "Key: Number Pad 3",
    "Key: Number Pad 6",
    // Skip index 120
    "Key: Number Pad .",
    // First multimedia key
    "Key: Media Previous",
    // Skip index 123
    // Skip index 124
    "Key: Number Pad -",
    "Key: Number Pad +",
    // Second multimedia key
    "Key: Media Play/Pause",
    // Third multimedia key
    "Key: Media Next",
    "Key: Number Pad Enter",
    "RGB Strip 1",
    "RGB Strip 2",
    "RGB Strip 3",
    "RGB Strip 4",
    "RGB Strip 5",
    "RGB Strip 6",
    "RGB Strip 7",
    "RGB Strip 8",
    "RGB Strip 9",
    "RGB Strip 10",
    "RGB Strip 11",
    "RGB Strip 12",
    "RGB Strip 13",
    "RGB Strip 14",
    "RGB Strip 15",
    "RGB Strip 16",
    "RGB Strip 17",
    "RGB Strip 18",
};

RGBController_HyperXAlloyElite2::RGBController_HyperXAlloyElite2(HyperXAlloyElite2Controller* hyperx_ptr)
{
    hyperx = hyperx_ptr;

    name        = "HyperX Alloy Elite2 Keyboard Device";
    vendor      = "HyperX";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "HyperX Alloy Elite2 Keyboard Device";
    location    = hyperx->GetDeviceLocation();
    serial      = hyperx->GetSerialString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0xFFFF;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();

    /*-----------------------------------------------------*\
    | The Corsair Lighting Node Pro requires a packet within|
    | 20 seconds of sending the lighting change in order    |
    | to not revert back into rainbow mode.  Start a thread |
    | to continuously send a keepalive packet every 5s      |
    \*-----------------------------------------------------*/
    keepalive_thread_run = true;
    keepalive_thread = new std::thread(&RGBController_HyperXAlloyElite2::KeepaliveThread, this);
}

RGBController_HyperXAlloyElite2::~RGBController_HyperXAlloyElite2()
{
    keepalive_thread_run = false;
    keepalive_thread->join();
    delete keepalive_thread;

    /*---------------------------------------------------------*\
    | Delete the matrix map                                     |
    \*---------------------------------------------------------*/
    for (unsigned int zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        if (zones[zone_index].matrix_map != nullptr)
        {
            delete zones[zone_index].matrix_map;
        }
    }

    delete hyperx;
}

void RGBController_HyperXAlloyElite2::SetupZones()
{
    /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    unsigned int total_led_count = 0;
    for(unsigned int zone_idx = 0; zone_idx < 1; zone_idx++)
    {
        zone new_zone;
        new_zone.name                   = zone_names[zone_idx];
        new_zone.type                   = zone_types[zone_idx];
        new_zone.leds_min               = zone_sizes[zone_idx];
        new_zone.leds_max               = zone_sizes[zone_idx];
        new_zone.leds_count             = zone_sizes[zone_idx];

        if(zone_types[zone_idx] == ZONE_TYPE_MATRIX)
        {
            new_zone.matrix_map         = new matrix_map_type;
            new_zone.matrix_map->height = 8;
            new_zone.matrix_map->width  = 22;
            new_zone.matrix_map->map    = (unsigned int *)&matrix_map;
        }
        else
        {
            new_zone.matrix_map         = nullptr;
        }

        zones.push_back(new_zone);

        total_led_count += zone_sizes[zone_idx];
    }

    for(unsigned int led_idx = 0; led_idx < total_led_count; led_idx++)
    {
        led new_led;
        new_led.name = led_names[led_idx];
        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_HyperXAlloyElite2::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_HyperXAlloyElite2::DeviceUpdateLEDs()
{
    {
      std::lock_guard<std::mutex> lock(last_update_time_mutex);
      last_update_time = std::chrono::steady_clock::now();
    }
    hyperx->SetLEDsDirect(colors);
}

void RGBController_HyperXAlloyElite2::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HyperXAlloyElite2::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_HyperXAlloyElite2::SetCustomMode()
{

}

void RGBController_HyperXAlloyElite2::DeviceUpdateMode()
{

}

/* Keepalive duration of 1 second */
const std::chrono::duration<long long int, std::ratio<1, 1000>> RGBController_HyperXAlloyElite2::send_delay = 1000ms;

void RGBController_HyperXAlloyElite2::KeepaliveThread()
{
    while(keepalive_thread_run.load())
    {
        if(active_mode == 0)
        {
            std::chrono::duration<long long int, std::ratio<1, 1000000000>> last_sent_delay{};
            {
                std::lock_guard<std::mutex> lock(last_update_time_mutex);
                last_sent_delay = std::chrono::steady_clock::now() - last_update_time;
            }
            if (last_sent_delay >= send_delay)
            {
                DeviceUpdateLEDs();
            }
            std::this_thread::sleep_for(20ms);
        }
    }
}
