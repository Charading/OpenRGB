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

    mode Off;
    Off.name       = "Off";
    Off.flags      = MODE_FLAG_HAS_PER_LED_COLOR;
    Off.color_mode = MODE_COLORS_PER_LED;
    modes.push_back(Off);

    SetupZones();
}

void RGBController_QMKXAP::SetupZones()
{
    VectorMatrix<uint16_t> keycode_map = controller->GetKeycodeMap();
    std::vector<XAPLED> xap_leds = controller->GetLEDs();
    VectorMatrix<unsigned int> matrix_map = PlaceLEDs(keycode_map, xap_leds);
    flat_matrix_map = FlattenMatrixMap(matrix_map);

    /*---------------------------------------------------------*\
    | Create Keyboard zone                                      |
    \*---------------------------------------------------------*/
    zone keys_zone;
    keys_zone.name                          = ZONE_EN_KEYBOARD;
    keys_zone.type                          = ZONE_TYPE_MATRIX;
    keys_zone.leds_min                      = xap_leds.size();
    keys_zone.leds_max                      = keys_zone.leds_min;
    keys_zone.leds_count                    = keys_zone.leds_min;
    keys_zone.matrix_map                    = new matrix_map_type;
    keys_zone.matrix_map->width             = keycode_map[0].size();
    keys_zone.matrix_map->height            = keycode_map.size();
    keys_zone.matrix_map->map               = flat_matrix_map.data();
    zones.push_back(keys_zone);


    /*---------------------------------------------------------*\
    | Create LEDs                                               |
    \*---------------------------------------------------------*/
    for (XAPLED xap_led : xap_leds)
    {
        led keyboard_led;

        keyboard_led.name = xap_led.label;
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
    VectorMatrix<unsigned int> matrix_map(keycodes.size(), std::vector<unsigned int>(keycodes[0].size(), NO_LED));
    unsigned int underglow_counter = 0;

    for (size_t i = 0; i < xap_leds.size(); i++)
    {
        if (xap_leds[i].flags & (LED_FLAG_KEYLIGHT | LED_FLAG_MODIFIER) && xap_leds[i].matrix_x >= 0)
        {
            matrix_map[xap_leds[i].matrix_y][xap_leds[i].matrix_x] = (unsigned int)i;
            xap_leds[i].label = QMKKeycodeToKeynameMap[keycodes[xap_leds[i].matrix_y][xap_leds[i].matrix_x]];
            LOG_TRACE("[QMK XAP] Setting matrix map (%d, %d) to %u", xap_leds[i].matrix_y, xap_leds[i].matrix_x, (unsigned int)i);
        }
        else if (xap_leds[i].flags & LED_FLAG_UNDERGLOW)
        {
            xap_leds[i].label = "Underglow " + std::to_string(underglow_counter);
            underglow_counter++;
        }
    }

    return matrix_map;
}
