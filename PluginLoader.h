#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <ResourceManager.h>
#include <QPluginLoader>

class Plugin
{
Q_INTERFACES(ORGBPluginInterface)
public:
    virtual ~Plugin() {}

    struct PluginDetails
    {
        std::string PluginName;
        std::string PluginDesc;
        std::string GUILocation;
    };
};

class PluginManager
{
public:
    std::vector<Plugin> ActivePlugins;
    std::vector<std::string> ActivePluginStrings;
    struct PluginReturn {QWidget *Contents; std::string destination;};
    enum ExtensionGUILocations {
        DEVICE_TAB,
        INFO_TAB,
        SETTINGS_TAB,
        TOP_TAB_BAR
    };

    void ScanForPlugins();
    void LoadPlugins(std::string FName, QWidget *Parent);
};

#endif // PLUGINLOADER_H
