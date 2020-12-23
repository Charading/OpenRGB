#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <QPluginLoader>
#include <QLabel>
#include <QtPlugin>
#include <QDir>

#include <string>
#include <iostream>
#include <ResourceManager.h>

#pragma once

/*-------------------------------------------------*\
| Options for needed data include                   |
|                                                   |
| 1. DarkTheme                                      |
| 2. Settings                                       |
| 3. Devices                                        |
| 4. Clients                                        |
|                                                   |
| Clients and devices will be static most likely    |
\*-------------------------------------------------*/

struct PluginDataNeeded
{
    std::vector<int>                        Needed;

    bool                                    DarkTheme;
    std::vector<std::vector<std::string>>   Settings;
    std::vector<RGBController *>            Devices;
    std::vector<NetworkClient *>            Clients;
};

#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual ~ORGBPluginInterface() {}

    virtual void DefineNeeded() = 0;

    PluginDataNeeded    NeededInfo;
    virtual bool        HasCustomIcon() = 0;
    virtual QLabel*     TabLabel()  = 0;

    virtual std::string PluginName()  = 0;
    virtual std::string PluginDesc()  = 0;
    virtual std::string PluginLocal()  = 0;

    virtual QWidget *CreateGUI(QWidget *Parent) = 0;
};

Q_DECLARE_INTERFACE(ORGBPluginInterface, ORGBPluginInterface_IID)

class PluginManager
{
public:
    std::vector<ORGBPluginInterface*> ActivePlugins;
    void SupplyNeedeedInfo(ORGBPluginInterface *ORGBPlgin,bool DarkTheme);

    void ScanAndLoadPlugins();
};

#endif // PLUGINLOADER_H
