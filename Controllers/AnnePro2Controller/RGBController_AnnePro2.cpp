#include "RGBController_AnnePro2.h"

#define NA  0xFFFFFFFF
#define LED_REAL_COUNT (5*14)
#define LED_COUNT (LED_REAL_COUNT - 9)

static unsigned int matrix_map[5][14] =
    { { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13 },
      { 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27 },
      { 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, NA },
      { 41, NA, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, NA }, 
      { 53, NA, 54, 55, NA, NA, 56, NA, NA, 57, 58, 59, 60, NA } };

static const char* zone_names[] =
{
    "Keyboard",
};

static zone_type zone_types[] =
{
    ZONE_TYPE_MATRIX
};

static const unsigned int zone_sizes[] =
{
    LED_COUNT,
};

typedef struct
{
    const char *        name;
    const unsigned char idx;
} led_type;

static const led_type led_names[] =
{
    /* Key Label                Index  */
    { "Key: Escape",            0       },
    { "Key: 1",                 1       },
    { "Key: 2",                 2       },
    { "Key: 3",                 3       },
    { "Key: 4",                 4       },
    { "Key: 5",                 5       },
    { "Key: 6",                 6       },
    { "Key: 7",                 7       },
    { "Key: 8",                 8       },
    { "Key: 9",                 9       },
    { "Key: 0",                 10      },
    { "Key: Minus",             11      },
    { "Key: Equals",            12      },
    { "Key: Backspace",         13      },
    { "Key: tab",               14      },
    { "Key: q",                 15      },
    { "Key: w",                 16      },
    { "Key: e",                 17      },
    { "Key: r",                 18      },
    { "Key: t",                 19      },
    { "Key: y",                 20      },
    { "Key: u",                 21      },
    { "Key: i",                 22      },
    { "Key: o",                 23      },
    { "Key: p",                 24      },
    { "Key: lbracket",          25      },
    { "Key: rbracket",          26      },
    { "Key: backslash",         27      },
    { "Key: caps",              28      },
    { "Key: a",                 29      },
    { "Key: s",                 30      },
    { "Key: d",                 31      },
    { "Key: f",                 32      },
    { "Key: g",                 33      },
    { "Key: h",                 34      },
    { "Key: j",                 35      },
    { "Key: k",                 36      },
    { "Key: l",                 37      },
    { "Key: semicolon",         38      },
    { "Key: apostrophe",        39      },
    { "Key: enter",             40      },
    { "Key: leftshift",         41      },
    { "Key: z",                 42      },
    { "Key: x",                 43      },
    { "Key: c",                 44      },
    { "Key: v",                 45      },
    { "Key: b",                 46      },
    { "Key: n",                 47      },
    { "Key: m",                 48      },
    { "Key: comma",             49      },
    { "Key: dot",               50      },
    { "Key: slash",             51      },
    { "Key: rightshift",        52      },
    { "Key: leftctrl",          53      },
    { "Key: leftsuper",         54      },
    { "Key: leftalt",           55      },
    { "Key: space",             56      },
    { "Key: rightalt",          57      },
    { "Key: fn",                58      },
    { "Key: context",           59      },
    { "Key: rightctrl",         60      },
};

RGBController_AnnePro2::RGBController_AnnePro2(AnnePro2Controller* annepro2_ptr) 
{
    annepro2 = annepro2_ptr;

    name        = "AnnePro2 Keyboard";
    vendor      = "Obins Lab";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Obins Lab AnnePro2 Keyboard Device";
    location    = annepro2->GetDeviceLocation();
    serial      = annepro2->GetSerialString();

    mode Direct;
    Direct.name       = "Direct";
    Direct.value      = 0;
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    SetupZones();
}

RGBController_AnnePro2::~RGBController_AnnePro2() 
{
    delete annepro2;
}

void RGBController_AnnePro2::SetupZones() 
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
            new_zone.matrix_map->height = 5;
            new_zone.matrix_map->width  = 14;
            new_zone.matrix_map->map    = (unsigned int *)&matrix_map;
        }
        else
        {
            new_zone.matrix_map         = NULL;
        }

        zones.push_back(new_zone);

        total_led_count += zone_sizes[zone_idx];
    }

    for(unsigned int led_idx = 0; led_idx < total_led_count; led_idx++)
    {
        led new_led;
        new_led.name  = led_names[led_idx].name;
        new_led.value = led_names[led_idx].idx;
        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_AnnePro2::ResizeZone(int zone, int new_size) 
{
    (void)zone;
    (void)new_size;
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_AnnePro2::DeviceUpdateLEDs() 
{
    const unsigned char frame_buf_length = LED_REAL_COUNT * 3;
    unsigned char frame_buf[frame_buf_length];

    /*---------------------------------------------------------*\
    | TODO: Send packets with multiple LED frames               |
    \*---------------------------------------------------------*/
    std::size_t led_real_idx = 0;
    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        frame_buf[(led_real_idx * 3) + 0] = RGBGetRValue(colors[led_idx]);
        frame_buf[(led_real_idx * 3) + 1] = RGBGetGValue(colors[led_idx]);
        frame_buf[(led_real_idx * 3) + 2] = RGBGetBValue(colors[led_idx]);

        if(led_idx == 40 || led_idx == 41 || led_idx == 52 || led_idx == 53 || led_idx == 60){
            led_real_idx++;
        } else if (led_idx == 55 || led_idx == 56){
            led_real_idx++;
            led_real_idx++;
        }

        led_real_idx++;
    }

    annepro2->SendDirect(frame_buf_length, frame_buf);
}

void RGBController_AnnePro2::UpdateZoneLEDs(int zone) 
{
    (void)zone;
    DeviceUpdateLEDs();
}

void RGBController_AnnePro2::UpdateSingleLED(int led) 
{
    (void)led;
    DeviceUpdateLEDs();
}

void RGBController_AnnePro2::SetCustomMode() 
{
    active_mode = 0;
}

void RGBController_AnnePro2::DeviceUpdateMode() 
{
    
}
