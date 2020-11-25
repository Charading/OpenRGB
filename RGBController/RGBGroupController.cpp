#include "RGBGroupController.h"

RGBGroupController::RGBGroupController(std::string _name, std::vector<RGBController *> _rgb_group)
{
    if (_rgb_group.size() < 2)
    {
        return;
    }

    name = _name;
    rgb_group = _rgb_group;
    type = DEVICE_TYPE_GROUP;
    SetupModes();
    SetupZones();
    SetupColors();

    /*---------------------------------------------------------*\
    | Initialize colors for each LED                            |
    \*---------------------------------------------------------*/
    for(std::size_t led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        unsigned char red = 127;
        unsigned char grn = 127;
        unsigned char blu = 127;

        colors[led_idx] = ToRGBColor(red, grn, blu);
    }
}

RGBGroupController::~RGBGroupController()
{

}

void RGBGroupController::SetupZones()
{
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        zone Group;
        Group.name          = rgb_group.at(dev_idx)->name.append(" Zone");
        Group.type          = rgb_group.at(dev_idx)->zones.at(0).type;
        Group.leds_min      = rgb_group.at(dev_idx)->zones.at(0).leds_min;
        Group.leds_max      = rgb_group.at(dev_idx)->zones.at(0).leds_max;
        Group.leds_count    = rgb_group.at(dev_idx)->zones.at(0).leds_count;
        Group.matrix_map    =  rgb_group.at(dev_idx)->zones.at(0).matrix_map;
        zones.push_back(Group);

        for(int i = Group.leds_min; i < Group.leds_max; i++)
        {
            led Group_led;
            Group_led.name = rgb_group.at(dev_idx)->name.append(" LED").append(std::to_string(i));
            Group_led.value = i;
            leds.push_back(Group_led);
        }
    }
}

void RGBGroupController::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBGroupController::DeviceUpdateLEDs()
{
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        rgb_group.at(dev_idx)->active_mode = active_mode;
        rgb_group.at(dev_idx)->colors.at(0) = colors.at(0);
        rgb_group.at(dev_idx)->modes.at(active_mode).value = modes.at(active_mode).value;
        if (rgb_group.at(dev_idx)->modes.at(active_mode).color_mode == MODE_COLORS_MODE_SPECIFIC)
        {
            int color_count = rgb_group.at(dev_idx)->modes.at(active_mode).colors_max;
            for(int i = 0; i < color_count; i++)
                rgb_group.at(dev_idx)->modes.at(active_mode).colors.at(i) = modes.at(active_mode).colors.at(i);
        }
        rgb_group.at(dev_idx)->modes.at(active_mode).direction = modes.at(active_mode).direction;
        rgb_group.at(dev_idx)->modes.at(active_mode).speed = modes.at(active_mode).speed;
        rgb_group.at(dev_idx)->modes.at(active_mode).color_mode = modes.at(active_mode).color_mode;

        rgb_group.at(dev_idx)->DeviceUpdateLEDs();
    }
}

void RGBGroupController::UpdateZoneLEDs(int zone)
{
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        rgb_group.at(dev_idx)->active_mode = active_mode;
        rgb_group.at(dev_idx)->colors.at(zone) = colors.at(zone);
        rgb_group.at(dev_idx)->modes.at(active_mode).value = modes.at(active_mode).value;
        if (rgb_group.at(dev_idx)->modes.at(active_mode).color_mode == MODE_COLORS_MODE_SPECIFIC)
        {
            int color_count = rgb_group.at(dev_idx)->modes.at(active_mode).colors_max;
            for(int i = 0; i < color_count; i++)
                rgb_group.at(dev_idx)->modes.at(active_mode).colors.at(i) = modes.at(active_mode).colors.at(i);
        }
        rgb_group.at(dev_idx)->modes.at(active_mode).direction = modes.at(active_mode).direction;
        rgb_group.at(dev_idx)->modes.at(active_mode).speed = modes.at(active_mode).speed;
        rgb_group.at(dev_idx)->modes.at(active_mode).color_mode = modes.at(active_mode).color_mode;

        rgb_group.at(dev_idx)->UpdateZoneLEDs(zone);
    }
}

void RGBGroupController::UpdateSingleLED(int led)
{
    DeviceUpdateLEDs();
}

void RGBGroupController::SetCustomMode()
{
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        rgb_group.at(dev_idx)->SetCustomMode();
    }
}

void RGBGroupController::DeviceUpdateMode()
{
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        rgb_group.at(dev_idx)->active_mode = active_mode;
        rgb_group.at(dev_idx)->colors.at(0) = colors.at(0);
        rgb_group.at(dev_idx)->modes.at(active_mode).value = modes.at(active_mode).value;
        if (rgb_group.at(dev_idx)->modes.at(active_mode).color_mode == MODE_COLORS_MODE_SPECIFIC)
        {
            int color_count = rgb_group.at(dev_idx)->modes.at(active_mode).colors_max;
            for(int i = 0; i < color_count; i++)
                rgb_group.at(dev_idx)->modes.at(active_mode).colors.at(i) = modes.at(active_mode).colors.at(i);
        }
        rgb_group.at(dev_idx)->modes.at(active_mode).direction = modes.at(active_mode).direction;
        rgb_group.at(dev_idx)->modes.at(active_mode).speed = modes.at(active_mode).speed;
        rgb_group.at(dev_idx)->modes.at(active_mode).color_mode = modes.at(active_mode).color_mode;

        rgb_group.at(dev_idx)->UpdateMode();
    }
}

void RGBGroupController::SetupModes()
{
    RGBController* most_modes;

    //Iterate the controllers to find the "Most Modes"
    uint8_t mm = 0;
    for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
    {
        if (rgb_group.at(dev_idx)->modes.size() > mm)
        {
            mm = rgb_group.at(dev_idx)->modes.size();
            most_modes = rgb_group.at(dev_idx);
        }
    }
    //Compare all the other controllers to "Most Modes"
    //and only push back the modes that all the controllers support
    for(std::size_t find_mode = 0; find_mode < most_modes->modes.size(); find_mode++)
    {
        bool found_mode = true;     //Found mode is true because it exists on the "Most_modes" controller
        for(std::size_t dev_idx = 0; dev_idx < rgb_group.size(); dev_idx++)
        {
            bool mode_match = false;    //Mode match is false till found on the other controller
            if (rgb_group.at(dev_idx) == most_modes)    //To save time don't iterate over "most_modes" controller
            {
                mode_match = true;
            }
            else
            {
                for(std::vector<mode>::iterator search = rgb_group.at(dev_idx)->modes.begin(); search != rgb_group.at(dev_idx)->modes.end(); ++search)
                {
                    if (search->name == most_modes->modes.at(find_mode).name)
                    {
                        mode_match = true;
                        break;
                    }
                }
            }
            found_mode = (found_mode && mode_match);    //If mode is found "AND" it to found
        }
        if (found_mode)
            modes.push_back(most_modes->modes.at(find_mode));     //If found mode remains true over all controllers push it onto the modes
    }
}
