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
    qDebug() << ORGBPlugin->NeededInfo.Needed.size();
    if (ORGBPlugin->NeededInfo.Needed.size() > 0) // Make sure that the plugin isn't independant
    {
        qDebug() << "Getting info for plugins";
        for (int i = 0; i < int(ORGBPlugin->NeededInfo.Needed.size()); i++) // For Needed item in the Needed vector
        {
            if (ORGBPlugin->NeededInfo.Needed[i] == 1) // If the setting is == 1 (Dark theme)
            {
                ORGBPlugin->NeededInfo.DarkTheme = DarkTheme; // Give it the theme setting
            }
            else if (ORGBPlugin->NeededInfo.Needed[i] == 2) // The needed setting == 2
            {
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
            }
            else if (ORGBPlugin->NeededInfo.Needed[i] == 3)
            {
                ORGBPlugin->NeededInfo.Devices = ResourceManager::get()->GetRGBControllers();
            }
            else if (ORGBPlugin->NeededInfo.Needed[i] == 4)
            {
                ORGBPlugin->NeededInfo.Clients = ResourceManager::get()->GetClients();
            }
        }
    }
}
