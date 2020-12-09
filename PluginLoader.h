#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <ResourceManager.h>
#include <QPluginLoader>

class ORGBPlugin
{
public:
    virtual ~ORGBPlugin() {}

    std::string PluginName;
    std::string PluginDesc;
    std::string PluginLocal;

    virtual QWidget* CreateGUI(QWidget *Parent);
};

class PluginManager
{
public:
    std::vector<ORGBPlugin> ActivePlugins;
    std::vector<std::string> ActivePluginStrings;

    void ScanAndLoadPlugins(QWidget *Parent);
};

#endif // PLUGINLOADER_H
