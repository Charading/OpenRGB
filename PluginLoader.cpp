#include "PluginLoader.h"
#include "ResourceManager.h"
#include "OpenRGBDialog2.h"

#include <QPluginLoader>
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

void PluginManager::ScanForPlugins()
{
    std::string OpenRGBConfigDir = ResourceManager::get()->GetConfigurationDirectory();

    std::string PluginPath = OpenRGBConfigDir + "/Plugins";

    PluginManager::ActivePluginStrings = FindPlugins(PluginPath.c_str());

    return;
}

void PluginManager::LoadPlugins(std::string FName, QWidget *Parent)
{

   QLibrary Plugin(QString().fromStdString(FName),Parent);

   if (Plugin.load())
       qDebug() << QString().fromStdString(FName) + " loaded";
   else
   {
       qDebug() << Plugin.errorString();
   }

   typedef QWidget (*PluginCreateGUI)(QWidget *Parent);
   PluginCreateGUI CreateGUI;

   CreateGUI = (PluginCreateGUI) Plugin.resolve("CreateGUI");

   if (CreateGUI)
   {
       qInfo() << "Function Discovered";
       CreateGUI(Parent);
   }
}
