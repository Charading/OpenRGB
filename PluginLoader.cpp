#include "PluginLoader.h"
#include "ResourceManager.h"
#include "OpenRGBDialog2.h"

#include <QPluginLoader>
#include <string>
#include <iostream>
#include <QDir>
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

void PluginManager::ScanAndLoadPlugins()
{
    std::string OpenRGBConfigDir = ResourceManager::get()->GetConfigurationDirectory();

    std::string PluginPath = OpenRGBConfigDir + "/Plugins";

    /*--------------------------------------------------------------------------------------*\
    | I used https://github.com/krf/cmake-qtqml-plugin-example to figure out how to do this  |
    | So BIG credit to krf                                                                   |
    \*--------------------------------------------------------------------------------------*/
    ORGBPluginInterface *ORGBPLugin = nullptr;

    // Should this get switched to dirent or is it ok as it is? Dirent add parent Dir and Current dir to the list so I starts at 2.
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
                break;
            }
        }
        else
        {
          qDebug() << loader.errorString();
        }
    }

    if (ORGBPLugin)
    {
        qDebug() << QString().fromStdString(ORGBPLugin->PluginName());
    }
    else
    {
    qWarning() << "Failed to find testplugin!";
    }
}
