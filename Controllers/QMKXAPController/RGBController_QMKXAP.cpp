/*-------------------------------------------------------------------*\
|  RGBController_QMKXAP.cpp                                           |
|                                                                     |
|  Driver for QMK keyboards using XAP                                 |
|                                                                     |
|  Jath03       11th August 2022                                      |
\*-------------------------------------------------------------------*/

#include "RGBController_QMKXAP.h"

RGBController_QMKXAP::RGBController_QMKXAP(QMKXAPController* controller_ptr)
{
    controller = controller_ptr;

    name = controller->GetName();
    vendor = controller->GetManufacturer();
    type = DEVICE_TYPE_KEYBOARD;
    description = "QMK Keyboard with XAP";
    location = controller->GetLocation();
    version = controller->GetVersion();
    serial = controller->GetHWID();

    mode Direct;
    Direct.name       = "Direct";
    Direct.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Off;
    Off.name        = "Off";
    Off.flags       = MODE_FLAG_MANUAL_SAVE;
    Off.color_mode  = MODE_COLORS_NONE;
    modes.push_back(Off);

    uint64_t effect_mask = controller->GetEnabledEffects();

    for (size_t i = 0; i < (controller->IsMatrix() ? RGBMatrixEffectNames : RGBLightEffectNames).size(); i++) {
        if (!((1ULL << i) & effect_mask)) continue;

        mode m;
        m.name = (controller->IsMatrix() ? RGBMatrixEffectNames : RGBLightEffectNames)[i];
        m.value = i;
        m.flags = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_MANUAL_SAVE;
        m.color_mode = MODE_COLORS_MODE_SPECIFIC;
        m.speed_min = 0;
        m.speed_max = 255;
        m.colors_min = 1;
        m.colors_max = 1;
        m.colors.resize(1);
        modes.push_back(m);
    }

    GetCurrentMode();
    SetupZones();
}

void RGBController_QMKXAP::SetupZones()
{
    if (controller->IsMatrix())
        SetupRGBMatrixZones();
    else
        SetupRGBLightZones();
}

void RGBController_QMKXAP::SetupRGBMatrixZones()
{
    VectorMatrix<uint16_t> keycode_map = controller->GetKeycodeMap();
    std::vector<XAPLED> xap_leds = controller->GetRGBMatrixLEDs();
    VectorMatrix<unsigned int> matrix_map = PlaceLEDs(keycode_map, xap_leds);
    flat_matrix_map = FlattenMatrixMap(matrix_map);

    /*---------------------------------------------------------*\
    | Create LEDs                                               |
    \*---------------------------------------------------------*/
    for (size_t i = 0; i < xap_leds.size(); i++)
    {
        led keyboard_led;

        keyboard_led.name = xap_leds[i].label;
        keyboard_led.value = i;

        leds.push_back(keyboard_led);
    }

    // Underglow
    unsigned int underglow_len = 0;
    for (size_t i = 0; i < xap_leds.size(); i++) {
        if (xap_leds[i].flags & 0x02) // Underglow flag
        {
            underglow_len++;
            std::rotate(xap_leds.begin() + i, xap_leds.begin() + i + 1, xap_leds.end()); // moves the underglow led to the back of the vector
        }
    }

    /*---------------------------------------------------------*\
    | Create Keyboard zone                                      |
    \*---------------------------------------------------------*/
    zone keys_zone;
    keys_zone.name                          = ZONE_EN_KEYBOARD;
    keys_zone.type                          = ZONE_TYPE_MATRIX;
    keys_zone.leds_min                      = xap_leds.size() - underglow_len;
    keys_zone.leds_max                      = keys_zone.leds_min;
    keys_zone.leds_count                    = keys_zone.leds_min;
    keys_zone.matrix_map                    = new matrix_map_type;
    keys_zone.matrix_map->width             = matrix_map[0].size();
    keys_zone.matrix_map->height            = matrix_map.size();
    keys_zone.matrix_map->map               = flat_matrix_map.data();
    zones.push_back(keys_zone);

    /*---------------------------------------------------------*\
    | Create Underglow zone                                     |
    \*---------------------------------------------------------*/

    if (underglow_len > 0)
    {
        zone underglow_zone;
        underglow_zone.name                 = "Underglow";
        underglow_zone.type                 = ZONE_TYPE_LINEAR;
        underglow_zone.leds_min             = underglow_len;
        underglow_zone.leds_max             = underglow_zone.leds_min;
        underglow_zone.leds_count           = underglow_zone.leds_min;
        zones.push_back(underglow_zone);
    }

    /*---------------------------------------------------------*\
    | Setup Colors                                              |
    \*---------------------------------------------------------*/
    SetupColors();
}

void RGBController_QMKXAP::SetupRGBLightZones()
{
    unsigned int xap_leds = controller->GetRGBLightLEDs();

    /*---------------------------------------------------------*\
    | Create Keyboard zone                                      |
    \*---------------------------------------------------------*/
    zone keys_zone;
    keys_zone.name                          = ZONE_EN_KEYBOARD;
    keys_zone.type                          = ZONE_TYPE_LINEAR;
    keys_zone.leds_min                      = xap_leds;
    keys_zone.leds_max                      = keys_zone.leds_min;
    keys_zone.leds_count                    = keys_zone.leds_min;
    zones.push_back(keys_zone);

    /*---------------------------------------------------------*\
    | Create LEDs                                               |
    \*---------------------------------------------------------*/
    for (size_t i = 0; i < xap_leds; i++)
    {
        led keyboard_led;

        keyboard_led.name = "LED " + std::to_string(i + 1);
        keyboard_led.value = 0;

        leds.push_back(keyboard_led);
    }

    /*---------------------------------------------------------*\
    | Setup Colors                                              |
    \*---------------------------------------------------------*/
    SetupColors();
}

void RGBController_QMKXAP::ResizeZone(int /*zone*/, int /*new_size*/)
{

}

void RGBController_QMKXAP::DeviceUpdateLEDs()
{

}

void RGBController_QMKXAP::UpdateZoneLEDs(int /*zone*/)
{

}

void RGBController_QMKXAP::UpdateSingleLED(int /*led*/)
{

}

void RGBController_QMKXAP::SetCustomMode()
{

}

void RGBController_QMKXAP::DeviceUpdateMode()
{
    if (active_mode == 0) // Direct mode not yet supported by xap
        return;
    else if (active_mode == 1)
        controller->SetEnabled(false);
    else {
        controller->SetMode(modes[active_mode].value, modes[active_mode].colors[0], modes[active_mode].speed);

        // QMK has one speed and color value for all modes.
        // This seems pretty inefficient, but represents
        // the device's functionality well.
        for (size_t i = 2; i < modes.size(); i++) {
            modes[i].colors[0] = modes[active_mode].colors[0];
            modes[i].speed = modes[active_mode].speed;
        }
    }
}

void RGBController_QMKXAP::DeviceSaveMode()
{
    controller->SaveMode();
}

std::vector<unsigned int> RGBController_QMKXAP::FlattenMatrixMap(VectorMatrix<unsigned int> matrix_map)
{
    std::vector<unsigned int> flat;

    for (std::vector<unsigned int> v : matrix_map)
    {
        flat.insert(flat.end(), v.begin(), v.end());
    }

    return flat;
}

VectorMatrix<unsigned int> RGBController_QMKXAP::PlaceLEDs(VectorMatrix<uint16_t> keycodes, std::vector<XAPLED>& xap_leds)
{
    size_t width = 0, height = 0;
    for (XAPLED led : xap_leds)
    {
        if (led.x > width)
            width = led.x;
        if (led.y > height)
            height = led.y;
    }
    VectorMatrix<unsigned int> matrix_map(height + 1, std::vector<unsigned int>(width + 1, NO_LED));
    unsigned int underglow_counter = 0;
    unsigned int unknown_counter = 0;

    for (unsigned int i = 0; i < (unsigned int)xap_leds.size(); i++)
    {
        if (xap_leds[i].flags & (LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER) && xap_leds[i].matrix_x >= 0)
        {
            matrix_map[xap_leds[i].y][xap_leds[i].x] = i - underglow_counter;
            try
            {
                xap_leds[i].label = QMKKeycodeToKeynameMap.at(keycodes[xap_leds[i].matrix_y][xap_leds[i].matrix_x]);

                // This is necessary because some random keycodes map to KEY_EN_UNUSED for some reason.
                if (xap_leds[i].label == "")
                    throw std::out_of_range("For fun");
            }
            catch (const std::out_of_range&)
            {
                xap_leds[i].label = "Key: Unknown " + std::to_string(unknown_counter + 1);
                unknown_counter++;
            }
            LOG_TRACE("[QMK XAP] Setting matrix map (%d, %d) to %u", xap_leds[i].y, xap_leds[i].x, i);
        }
        else if (xap_leds[i].flags & LED_FLAG_UNDERGLOW)
        {
            xap_leds[i].label = "Underglow " + std::to_string(underglow_counter + 1);
            underglow_counter++;
        }
    }

    return matrix_map;
}

void RGBController_QMKXAP::GetCurrentMode()
{
    XAPRGBConfig config = controller->GetRGBConfig();

    hsv_t color_hsv = {
        ((unsigned int)((config.hue / 255.0)) * 359),
        config.sat,
        (uint8_t)((config.val / (float)controller->GetMaxBrightness()) * 255)
    };

    for (size_t i = 2; i < modes.size(); i++)
    {
        modes[i].colors[0] = hsv2rgb(&color_hsv);
        modes[i].speed = config.speed;
        if (modes[i].value == config.mode)
        {
            active_mode = config.enable ? i : 1; // 1 is off mode
        }
    }
}
