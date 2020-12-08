#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <ResourceManager.h>
#include <QPluginLoader>

class Plugin
{
public:
    struct PluginDetails
    {
        std::string PluginName;
        std::string PluginDesc;
        std::vector<std::string> NeededHeaders;
        std::string GUILocation;
    };
};

class PluginManager
{
public:
    std::vector<Plugin> ActivePlugins;

    enum ExtensionGUILocations {
        DEVICE_TAB,
        INFO_TAB,
        SETTINGS_TAB,
        TOP_TAB_BAR
    };
private:
    void ScanForPlugins();
};

#endif // PLUGINLOADER_H
