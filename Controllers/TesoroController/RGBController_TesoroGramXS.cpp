/*-----------------------------------------*\
|  RGBController_TesoroGramXS.cpp           |
|                                           |
|  RGB Interface for Tesoro Gram XS         |
|  Keyboard                                 |
|                                           |
|  Ondřej Niesner 8/31/2023                 |
\*-----------------------------------------*/


#include "RGBControllerKeyNames.h"
#include "RGBController_TesoroGramXS.h"

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

static unsigned int matrix_map[6][21] =
    { {  0,   NA,  1,    2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,  NA,  NA,  NA,  NA, },
      {  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36, },
      {  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57, },
      {  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  NA,  70,  NA,  NA,  NA,  71,  72,  73,  NA, },
      {  74,  NA,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  NA,  85,  NA,  86,  NA,  87,  88,  89,  90, },
      {  91,  92,  93,  NA,  NA,  NA,  94,  NA,  NA,  NA,  95,  96,  97,  98,  99, 100, 101,  NA, 102,  103, NA  } };

static const char *led_names[] =
    {
        KEY_EN_ESCAPE,
        KEY_EN_F1,
        KEY_EN_F2,
        KEY_EN_F3,
        KEY_EN_F4,
        KEY_EN_F5,
        KEY_EN_F6,
        KEY_EN_F7,
        KEY_EN_F8,
        KEY_EN_F9,
        KEY_EN_F10,
        KEY_EN_F11,
        KEY_EN_F12,
        KEY_EN_PRINT_SCREEN,
        KEY_EN_SCROLL_LOCK,
        KEY_EN_PAUSE_BREAK,
        KEY_EN_BACK_TICK,
        KEY_EN_1,
        KEY_EN_2,
        KEY_EN_3,
        KEY_EN_4,
        KEY_EN_5,
        KEY_EN_6,
        KEY_EN_7,
        KEY_EN_8,
        KEY_EN_9,
        KEY_EN_0,
        KEY_EN_MINUS,
        KEY_EN_EQUALS,
        KEY_EN_BACKSPACE,
        KEY_EN_INSERT,
        KEY_EN_HOME,
        KEY_EN_PAGE_UP,
        KEY_EN_NUMPAD_LOCK,
        KEY_EN_NUMPAD_DIVIDE,
        KEY_EN_NUMPAD_TIMES,
        KEY_EN_NUMPAD_MINUS,
        KEY_EN_TAB,
        KEY_EN_Q,
        KEY_EN_W,
        KEY_EN_E,
        KEY_EN_R,
        KEY_EN_T,
        KEY_EN_Y,
        KEY_EN_U,
        KEY_EN_I,
        KEY_EN_O,
        KEY_EN_P,
        KEY_EN_LEFT_BRACKET,
        KEY_EN_RIGHT_BRACKET,
        KEY_EN_ANSI_BACK_SLASH,
        KEY_EN_DELETE,
        KEY_EN_END,
        KEY_EN_PAGE_DOWN,
        KEY_EN_NUMPAD_7,
        KEY_EN_NUMPAD_8,
        KEY_EN_NUMPAD_9,
        KEY_EN_NUMPAD_PLUS,
        KEY_EN_CAPS_LOCK,
        KEY_EN_A,
        KEY_EN_S,
        KEY_EN_D,
        KEY_EN_F,
        KEY_EN_G,
        KEY_EN_H,
        KEY_EN_J,
        KEY_EN_K,
        KEY_EN_L,
        KEY_EN_SEMICOLON,
        KEY_EN_QUOTE,
        KEY_EN_ANSI_ENTER,
        KEY_EN_NUMPAD_4,
        KEY_EN_NUMPAD_5,
        KEY_EN_NUMPAD_6,
        KEY_EN_LEFT_SHIFT,
        KEY_EN_Z,
        KEY_EN_X,
        KEY_EN_C,
        KEY_EN_V,
        KEY_EN_B,
        KEY_EN_N,
        KEY_EN_M,
        KEY_EN_COMMA,
        KEY_EN_PERIOD,
        KEY_EN_FORWARD_SLASH,
        KEY_EN_RIGHT_SHIFT,
        KEY_EN_UP_ARROW,
        KEY_EN_NUMPAD_1,
        KEY_EN_NUMPAD_2,
        KEY_EN_NUMPAD_3,
        KEY_EN_NUMPAD_ENTER,
        KEY_EN_LEFT_CONTROL,
        KEY_EN_LEFT_WINDOWS,
        KEY_EN_LEFT_ALT,
        KEY_EN_SPACE,
        KEY_EN_RIGHT_ALT,
        KEY_EN_RIGHT_FUNCTION,
        KEY_EN_MENU,
        KEY_EN_RIGHT_CONTROL,
        KEY_EN_LEFT_ARROW,
        KEY_EN_DOWN_ARROW,
        KEY_EN_RIGHT_ARROW,
        KEY_EN_NUMPAD_0,
        KEY_EN_NUMPAD_PERIOD
};

/**------------------------------------------------------------------*\
    @name Tesoro Gram XS
    @category Keyboard
    @type USB
    @save :robot:
    @direct :white_check_mark:
    @effects :white_check_mark:
    @detectors DetectTesoroGramXS
    @comment
\*-------------------------------------------------------------------*/

RGBController_TesoroGramXS::RGBController_TesoroGramXS(TesoroGramXSController *controller_ptr)
{
    controller  = controller_ptr;

    name        = "Tesoro Gram XS";
    vendor      = "Tesoro";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Tesoro Keyboard Device";
    serial      = controller->GetSerialString();
    location    = controller->GetLocation();

    mode Direct;
    Direct.name         = "Direct";
    Direct.value        = GRAM_XS_MODE_DIRECT;
    Direct.flags        = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode   = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Static;
    Static.name             = "Static";
    Static.value            = GRAM_XS_MODE_STATIC;
    Static.flags            = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Static.brightness       = GRAM_XS_BRIGHTNESS;
    Static.brightness_max   = GRAM_XS_BRIGHTNESS_MAX;
    Static.brightness_min   = GRAM_XS_BRIGHTNESS_MIN;
    Static.color_mode       = MODE_COLORS_PER_LED;
    modes.push_back(Static);

    mode Trigger;
    Trigger.name = "Trigger";
    Trigger.value               = GRAM_XS_MODE_TRIGGER;
    Trigger.flags               = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Trigger.brightness          = GRAM_XS_BRIGHTNESS;
    Trigger.brightness_max      = GRAM_XS_BRIGHTNESS_MAX;
    Trigger.brightness_min      = GRAM_XS_BRIGHTNESS_MIN;
    Trigger.color_mode          = MODE_COLORS_MODE_SPECIFIC;
    Trigger.colors_min          = 2;
    Trigger.colors_max          = 2;
    Trigger.colors.resize(2);
    modes.push_back(Trigger);

    mode Firework;
    Firework.name = "Firework";
    Firework.value              = GRAM_XS_MODE_FIREWORK;
    Firework.flags              = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Firework.brightness         = GRAM_XS_BRIGHTNESS;
    Firework.brightness_max     = GRAM_XS_BRIGHTNESS_MAX;
    Firework.brightness_min     = GRAM_XS_BRIGHTNESS_MIN;
    Firework.color_mode         = MODE_COLORS_MODE_SPECIFIC;
    Firework.colors_min         = 2;
    Firework.colors_max         = 2;
    Firework.colors.resize(2);
    modes.push_back(Firework);

    mode Breathing;
    Breathing.name = "Breathing";
    Breathing.value             = GRAM_XS_MODE_BREATHING;
    Breathing.flags             = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Breathing.brightness        = GRAM_XS_BRIGHTNESS;
    Breathing.brightness_max    = GRAM_XS_BRIGHTNESS_MAX;
    Breathing.brightness_min    = GRAM_XS_BRIGHTNESS_MIN;
    Breathing.color_mode        = MODE_COLORS_MODE_SPECIFIC;
    Breathing.colors_min        = 2;
    Breathing.colors_max        = 2;
    Breathing.colors.resize(2);
    modes.push_back(Breathing);

    mode Star_Shining;
    Star_Shining.name           = "Star Shining";
    Star_Shining.value          = GRAM_XS_MODE_STAR_SHINING;
    Star_Shining.flags          = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Star_Shining.brightness     = GRAM_XS_BRIGHTNESS;
    Star_Shining.brightness_max = GRAM_XS_BRIGHTNESS_MAX;
    Star_Shining.brightness_min = GRAM_XS_BRIGHTNESS_MIN;
    Star_Shining.color_mode     = MODE_COLORS_MODE_SPECIFIC;
    Star_Shining.colors_min     = 2;
    Star_Shining.colors_max     = 2;
    Star_Shining.colors.resize(2);
    modes.push_back(Star_Shining);

    mode Cycling;
    Cycling.name = "Cycling";
    Cycling.value           = GRAM_XS_MODE_CYCLING;
    Cycling.flags           = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Cycling.brightness      = GRAM_XS_BRIGHTNESS;
    Cycling.brightness_max  = GRAM_XS_BRIGHTNESS_MAX;
    Cycling.brightness_min  = GRAM_XS_BRIGHTNESS_MIN;
    Cycling.color_mode      = MODE_COLORS_NONE;
    modes.push_back(Cycling);

    mode Ripple;
    Ripple.name             = "Ripple";
    Ripple.value            = GRAM_XS_MODE_RIPPLE;
    Ripple.flags            = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Ripple.brightness       = GRAM_XS_BRIGHTNESS;
    Ripple.brightness_max   = GRAM_XS_BRIGHTNESS_MAX;
    Ripple.brightness_min   = GRAM_XS_BRIGHTNESS_MIN;
    Ripple.color_mode       = MODE_COLORS_MODE_SPECIFIC;
    Ripple.colors_min       = 2;
    Ripple.colors_max       = 2;
    Ripple.colors.resize(2);
    modes.push_back(Ripple);

    mode Wave;
    Wave.name = "Wave";
    Wave.value          = GRAM_XS_MODE_WAVE;
    Wave.flags          = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Wave.brightness     = GRAM_XS_BRIGHTNESS;
    Wave.brightness_max = GRAM_XS_BRIGHTNESS_MAX;
    Wave.brightness_min = GRAM_XS_BRIGHTNESS_MIN;
    Wave.color_mode     = MODE_COLORS_NONE;
    modes.push_back(Wave);

    mode Spiral;
    Spiral.name             = "Spiral";
    Spiral.value            = GRAM_XS_MODE_SPIRAL;
    Spiral.flags            = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Spiral.brightness       = GRAM_XS_BRIGHTNESS;
    Spiral.brightness_max   = GRAM_XS_BRIGHTNESS_MAX;
    Spiral.brightness_min   = GRAM_XS_BRIGHTNESS_MIN;
    Spiral.color_mode       = MODE_COLORS_NONE;
    modes.push_back(Spiral);

    SetupZones();
}

RGBController_TesoroGramXS::~RGBController_TesoroGramXS()
{
    /*---------------------------------------------------------*\
    | Delete the matrix map                                     |
    \*---------------------------------------------------------*/
    for(unsigned int zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        if(zones[zone_index].matrix_map != NULL)
        {
            delete zones[zone_index].matrix_map;
        }
    }

    delete controller;
}

void RGBController_TesoroGramXS::SetupZones()
{
    zone XS_zone;
    XS_zone.name        = ZONE_EN_KEYBOARD;
    XS_zone.type        = ZONE_TYPE_MATRIX;
    XS_zone.leds_min    = GRAM_XS_KEY_COUNT;
    XS_zone.leds_max    = GRAM_XS_KEY_COUNT;
    XS_zone.leds_count  = GRAM_XS_KEY_COUNT;

    XS_zone.matrix_map          = new matrix_map_type;
    XS_zone.matrix_map->height  = 6;
    XS_zone.matrix_map->width   = 21;
    XS_zone.matrix_map->map     = (unsigned int *)&matrix_map;
    zones.push_back(XS_zone);

    for(unsigned int led_index = 0; led_index < GRAM_XS_KEY_COUNT; led_index++)
    {
        led XS_led;
        XS_led.name = led_names[led_index];
        XS_led.value = led_index;
        leds.push_back(XS_led);
    }

    SetupColors();
}

void RGBController_TesoroGramXS::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_TesoroGramXS::DeviceUpdateLEDs()
{
    if(modes[active_mode].value == GRAM_XS_MODE_STATIC)
    {
        controller->UpdateLeds(colors, true);
    }
    else
    {
        controller->UpdateLeds(colors);
    }
}

void RGBController_TesoroGramXS::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_TesoroGramXS::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_TesoroGramXS::DeviceUpdateMode()
{
    if(modes[active_mode].value != GRAM_XS_MODE_DIRECT)
    {
        controller->SetMode
                (
                modes[active_mode].value,
                modes[active_mode].brightness,
                modes[active_mode].color_mode,
                modes[active_mode].colors
                );
    }
}
