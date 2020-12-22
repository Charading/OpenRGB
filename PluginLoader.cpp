#include "PluginLoader.h"
void PluginManager::ScanAndLoadPlugins()
{
    std::string OpenRGBConfigDir = ResourceManager::get()->GetConfigurationDirectory();

    std::string PluginPath = OpenRGBConfigDir + "/Plugins";

    /*--------------------------------------------------------------------------------------*\
    | I used https://github.com/krf/cmake-qtqml-plugin-example to figure out how to do this  |
    | So BIG credit to krf                                                                   |
    \*--------------------------------------------------------------------------------------*/
    ORGBPluginInterface *ORGBPLugin = nullptr;

    const QDir pluginsDir = QString().fromStdString(ResourceManager::get()->GetConfigurationDirectory()) + "plugins/";

    std::vector<std::string> FileList;
    for (int i = 0; i < QDir(pluginsDir).entryList(QDir::Files).size(); i++)
    {
        /*--------------------------------------*\
        | Add all of the Plugin Files to a list  |
        \*--------------------------------------*/
        FileList.push_back(QDir(pluginsDir).entryList(QDir::Files)[i].toStdString());
    }

    for (const std::string &fileName : FileList)
    {
        const std::string filePath = pluginsDir.absoluteFilePath(QString().fromStdString(fileName)).toStdString();
        QPluginLoader loader(pluginsDir.absoluteFilePath(QString().fromStdString(fileName)));
        if (QObject *instance = loader.instance())
        {
            if ((ORGBPLugin = qobject_cast<ORGBPluginInterface*>(instance)))
            {
                PluginManager::ActivePlugins.push_back(ORGBPLugin);
            }
        }
        else{}
    }
}

void PluginManager::SupplyNeedeedInfo(ORGBPluginInterface *ORGBPlugin,bool DarkTheme)
{
    ORGBPlugin->DefineNeeded();

    ORGBPlugin->NeededInfo.DarkTheme = DarkTheme; // Give it the theme setting
    qDebug() << "On the search for settings";
    if (ORGBPlugin->NeededInfo.Settings.size() > 0) // Make sure that the settings vector contains an actual setting to get
    {
        // In the case that they need multiple settings it should be able to get all of them
        std::vector<std::vector<std::string>> SettingsToGet = ORGBPlugin->NeededInfo.Settings; // Make a backup of the settings so that it can just replace the variable
        for (int SubI = 0; SubI < int(SettingsToGet.size()); SubI++) // For Setting in the Settings Vector
        {
            if (SettingsToGet[SubI].size() > 1) // Check First setting vector size
            {
                json GetSetting;
                GetSetting = ResourceManager::get()->GetSettingsManager()->GetSettings(SettingsToGet[SubI][0]); // Get the setting it should be for further info
                for (int SubSubI = 1; SubSubI < int(SettingsToGet[SubI].size()); SubSubI++) // For setting under setting name
                {
                    if (GetSetting.contains(SettingsToGet[SubI][SubSubI]))
                    {
                        std::string GrabSetting = GetSetting[SettingsToGet[SubI][SubSubI]];
                        ORGBPlugin->NeededInfo.Settings[SubI][SubSubI] = GetSetting[GrabSetting];
                        qDebug() << QString().fromStdString(GrabSetting);
                    }
                    else
                    {
                        qDebug() << "no setting found";
                        ORGBPlugin->NeededInfo.Settings[SubI][SubSubI] = "NoSettingFound";
                    }
                }
            }
        }
    }
    ORGBPlugin->NeededInfo.Devices = ResourceManager::get()->GetRGBControllers();
    ORGBPlugin->NeededInfo.Clients = ResourceManager::get()->GetClients();

    return;
}
