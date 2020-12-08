#include "PluginLoader.h"
#include "ResourceManager.h"

/*-------------------------------------------------------------------------------------------------------------------------*\
| Lets start by defining some things                                                                                        |
| 1. All plugins get Headers THROUGH plugin loader                                                                          |
|       They can't go just importing things at random                                                                       |
| 2. For Plugins that need file operations, They MUST be granted by the user and even then only done through pluginloader   |
|       (Think effect running)                                                                                              |
| 3. The plugin MUST define where in the GUI it goes.                                                                       |
|       This will be done through a struct labeled PluginDetails                                                            |
\*-------------------------------------------------------------------------------------------------------------------------*/

#include <string>
#include <iostream>
#include <dependencies/dirent.h>

#ifdef _WIN32

#else
#include <dirent.h>
#endif

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

    std::vector<std::string> OutList;
    OutList = FindPlugins(OpenRGBConfigDir.c_str());
    for (int i = 0; i < int(OutList.size()); i++)
    {
        std::cout << OutList[i];
    }
    std::string PluginPath = OpenRGBConfigDir + "/Plugins";

}

