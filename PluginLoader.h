#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <ResourceManager.h>
#include <QPluginLoader>
#include <QLabel>
#include <QtPlugin>

#pragma once

class QString;
#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual ~ORGBPluginInterface() {}

    virtual bool        HasCustomIcon() const = 0;
    virtual QLabel*     TabLabel()      const = 0;

    virtual std::string PluginName()    const = 0;
    virtual std::string PluginDesc()    const = 0;
    virtual std::string PluginLocal()   const = 0;

    virtual QWidget *CreateGUI(QWidget *Parent, ResourceManager *RM, bool DarkTheme) const = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)

class PluginManager
{
public:
    std::vector<ORGBPluginInterface*> ActivePlugins;

    void ScanAndLoadPlugins();
};

#endif // PLUGINLOADER_H
