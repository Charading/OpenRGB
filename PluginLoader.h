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

struct PluginInfo
{
    std::string                 PluginName;
    std::string                 PluginDesc;
    std::string                 PluginLoca;

    bool                        HasCustom;
    QLabel                      *PluginLabel;

    std::string                 SettingName;
};

#define ORGBPluginInterface_IID "com.ORGBPluginInterface"

class ORGBPluginInterface
{
public:
    virtual         ~ORGBPluginInterface() {}

    PluginInfo              PInfo;

    virtual PluginInfo      DefineNeeded() = 0;

    virtual PluginInfo      init(json Settings , bool DarkTheme, ResourceManager *RM) = 0;

    virtual QWidget         *CreateGUI(QWidget *Parent) = 0;
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
