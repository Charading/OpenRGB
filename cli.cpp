#include <vector>
#include <cstring>
#include <string>
#include <tuple>
#include <iostream>
#include "OpenRGB.h"
#include "RGBController.h"
#include "i2c_smbus.h"

extern std::vector<i2c_smbus_interface*> busses;
extern std::vector<RGBController*> rgb_controllers;

struct Options
{
    int device = -1;
    std::vector<std::tuple<unsigned char, unsigned char, unsigned char>> colors;
    unsigned char mode;
    bool direct = true;
};

void PrintHelp();

// Takes a comma separated list of six digit rgb colors and the Options
// object to populate with the colors from the string.
// Returns true if at least one color tuple in options was populated
bool ParseColors(std::string colors_string, Options *options);

// Returns true if an effect could be parsed
bool ParseEffects(std::string effect, Options *options);

// Takes the program's arguments and populates res with the options
// given by the user
bool ProcessOptions(int argc, char *argv[], Options *res);

void ApplyOptions(int device_index, Options& options);

void PrintHelp()
{
    std::string help_text;
    help_text += "OpenRGB, for controlling RGB lighting.\n";
    help_text += "Usage: OpenRGB [options]\n";
    help_text += "\nOptions:\n";
    help_text += "--gui\t\t\t\t\t Show GUI, also appears when not passing any parameters\n";
    help_text += "-l,  --list-devices\t\t Lists every compatible device with their number\n";
    help_text += "-d,  --device [0-9]\t\t Selects device to apply colors and/or effect to, or applies to all devices if omitted\n";
    help_text += "-c,  --color \"FFFFFF,00AAFF...\"\t\t Sets colors on each device directly if no effect is specified, and sets the effect color if an effect is specified\n";
    help_text += "\t\t If there are more LEDs than colors given, the last color will be applied to the remaining LEDs\n";
    help_text += "-e,  --effect [breathing | static | ...]\t\t Sets the effect to be applied\n";
    help_text += "\nEffects:\n";
    help_text += "  off\n  static\n  breathing\n  flashing\n  spectrum\n  rainbow\n  spectrum-breathing\n  chase";

    std::cout << help_text << std::endl;
}

bool ParseColors(std::string colors_string, Options *options)
{
    while (colors_string.length() >= 6)
    {
        int rgb_end = colors_string.find_first_of(',');
        std::string color = colors_string.substr(0, rgb_end);
        if (color.length() != 6)
            break;

        try
        {
            unsigned char r = std::stoi(color.substr(0, 2), nullptr, 16);
            unsigned char g = std::stoi(color.substr(2, 2), nullptr, 16);
            unsigned char b = std::stoi(color.substr(4, 2), nullptr, 16);
            options->colors.push_back(std::make_tuple(r, g, b));
        }
        catch (...)
        {
            break;
        }

        // If there are no more colors
        if (rgb_end == std::string::npos)
            break;
        // Remove the current color and the next color's leading comma
        colors_string = colors_string.substr(color.length() + 1);
    }

    return options->colors.size() > 0;
}

bool ParseEffect(std::string effect, Options *options)
{
    options->direct = false;
    options->mode = 0;

    return true;
}

std::string QuoteIfNecessary(std::string str)
{
    if (str.find(' ') == std::string::npos)
    {
        return str;
    }
    else
    {
        return "'" + str + "'";
    }
}

bool ProcessOptions(int argc, char *argv[], Options *res)
{
    bool color_init = false;
    bool effect_init = false;
    int arg_index = 1;

    while (arg_index < argc)
    {
        std::string option = argv[arg_index];

        // Handle options that take no arguments
        if (option == "--list-devices" || option == "-l")
        {
            DetectRGBControllers();
            for (int i = 0; i < rgb_controllers.size(); i++)
            {
                RGBController *c = rgb_controllers[i];
                std::cout << i << ": " << c->name << std::endl;

                std::cout << "  Type: " << device_type_to_str(c->type) << std::endl;

                if (!c->serial.empty())
                {
                    std::cout << "  Serial:      " << c->serial << std::endl;
                }

                if (!c->description.empty())
                {
                    std::cout << "  Description: " << c->description << std::endl;
                }

                if (!c->modes.empty())
                {
                    std::cout << "  Modes:";
                    int curMode = c->GetMode();
                    for (int i = 0; i < c->modes.size(); i++)
                    {
                        std::string modeStr = QuoteIfNecessary(c->modes[i].name);
                        if (curMode == i) {
                            modeStr = "[" + modeStr + "]";
                        }
                        std::cout << " " << modeStr;
                    }
                    std::cout << std::endl;
                }

                if (!c->zones.empty())
                {
                    std::cout << "  Zones:";
                    for (int i = 0; i < c->zones.size(); i++)
                    {
                        std::cout << " " << QuoteIfNecessary(c->zones[i].name);
                    }
                    std::cout << std::endl;
                }

                if (!c->leds.empty())
                {
                    std::cout << "  LEDs:";
                    for (int i = 0; i < c->leds.size(); i++)
                    {
                        std::cout << " " << QuoteIfNecessary(c->leds[i].name);
                    }
                    std::cout << std::endl;
                }

                std::cout << std::endl;
            }
            exit(0);
        }
        else if (arg_index + 1 < argc) // Handle options that take an argument
        {
            std::string argument = argv[arg_index + 1];

            if (option == "--device" || option == "-d")
            {
                try
                {
                    res->device = std::stoi(argument);
                }
                catch (...)
                {
                    return false;
                }
            }
            else if (option == "--color" || option == "-c")
            {
                if (ParseColors(argument, res))
                    color_init = true;
            }
            else if (option == "--effect" || option == "-e")
            {
                if (ParseEffect(argument, res))
                    effect_init = true;
            }
            else
            {
                return false;
            }

            arg_index++;
        }
        else
        {
            return false;
        }
        arg_index++;
    }

    return color_init || effect_init;
}

void ApplyOptions(int device_index, Options& options)
{
    RGBController *device = rgb_controllers[device_index];
    if (options.colors.size() != 0)
    {
        int last_set_color;
        for (int i = 0; i < device->leds.size(); i++)
        {
            if (i < options.colors.size())
            {
                last_set_color = i;
            }

            device->SetLED(i, ToRGBColor(std::get<0>(options.colors[last_set_color]),
                                         std::get<1>(options.colors[last_set_color]),
                                         std::get<2>(options.colors[last_set_color])));
        }
    }

    if (!options.direct) {
        device->SetMode(options.mode);
    }
}

int cli_main(int argc, char *argv[])
{
    if (argc == 2 && (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")))
    {
        PrintHelp();
        return 0;
    }

    Options options;
    if (!ProcessOptions(argc, argv, &options))
    {
        PrintHelp();
        return -1;
    }

    DetectRGBControllers();

    if (options.device == -1)
    {
        for (int i = 0; i < rgb_controllers.size(); i++)
        {
            ApplyOptions(i, options);
        }
    }
    else
    {
        ApplyOptions(options.device, options);
    }

    return 0;
}
