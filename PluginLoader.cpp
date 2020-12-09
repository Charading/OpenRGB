#include "PluginLoader.h"
#include "ResourceManager.h"
#include "OpenRGBDialog2.h"

#include <QPluginLoader>
#include <QtPlugin>
#include <string>
#include <iostream>
#include <dependencies/dirent.h>

std::vector<std::string> FindPlugins(const char *path)
{
   struct dirent *entry;
   std::vector<std::string> PossiblePluggins;
   DIR *dir = opendir(path);

   if (dir == NULL) {
       return {};
   }
   while ((entry = readdir(dir)) != NULL)
   {
       PossiblePluggins.push_back(entry->d_name);
   }
   closedir(dir);
   return PossiblePluggins;
}

void PluginManager::ScanAndLoadPlugins(QWidget *Parent)
{
    std::string OpenRGBConfigDir = ResourceManager::get()->GetConfigurationDirectory();

    std::string PluginPath = OpenRGBConfigDir + "/Plugins";

    PluginManager::ActivePluginStrings = FindPlugins(PluginPath.c_str());

    if (int(PluginManager::ActivePluginStrings.size()) > 2)
    {
        for (int i = 2; i < int(PluginManager::ActivePluginStrings.size()); i++)
        {
            QPluginLoader PLGN(QString().fromStdString(PluginManager::ActivePluginStrings[i]),Parent);
            if (PLGN.load())
            {
                qDebug() << "Loaded " + QString().fromStdString(PluginManager::ActivePluginStrings[i]);
                //ORGBPlugin::CreateGUI(Parent)
            }
            else
            {
                qDebug() << "Failed to load plugin " + QString().fromStdString(PluginManager::ActivePluginStrings[i]);
            }
            //Plugin *NewPlug = new Plugin;
            //QLibrary PlugInfo(QString().fromStdString(ActivePluginStrings[i]));

            //PluginManager::ActivePlugins.push_back()
        }
    }
}
