#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <ResourceManager.h>
#include <QPluginLoader>
#include <QtPlugin>

#pragma once

class QString;
#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual ~ORGBPluginInterface() {}

    virtual std::string PluginName()  const = 0;
    virtual std::string PluginDesc()  const = 0;
    virtual std::string PluginLocal() const = 0;

    virtual QWidget *CreateGUI(QWidget *Parent, ResourceManager *RM = nullptr) const = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)


class PluginManager
{
public:
    std::vector<ORGBPluginInterface*> ActivePlugins;

    void ScanAndLoadPlugins();
};

#endif // PLUGINLOADER_H
