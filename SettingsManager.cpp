/*-----------------------------------------*\
|  SettingsManager.cpp                      |
|                                           |
|  OpenRGB Settings Manager maintains a list|
|  of application settings in JSON format.  |
|  Other components may register settings   |
|  with this class and store/load values.   |
|                                           |
|  Adam Honse (CalcProgrammer1) 11/4/2020   |
\*-----------------------------------------*/

#include "SettingsManager.h"
#include "LogManager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include "filesystem.h"

SettingsManager::SettingsManager()
{

}

SettingsManager::~SettingsManager()
{

}

json SettingsManager::GetSettings(std::string settings_key)
{
    /*---------------------------------------------------------*\
    | Check to see if the key exists in the settings store and  |
    | return the settings associated with the key if it exists  |
    \*---------------------------------------------------------*/
    if(settings_data.contains(settings_key))
    {
        return(settings_data[settings_key]);
    }

    /*---------------------------------------------------------*\
    | If the settings store doesn't contain the key, create an  |
    | empty json and return it                                  |
    \*---------------------------------------------------------*/
    json empty;

    return(empty);
}

void SettingsManager::SetSettings(std::string settings_key, json new_settings)
{
    settings_data[settings_key] = new_settings;
}

void SettingsManager::LoadSettings(const filesystem::path& filename)
{
    LOG_INFO("[SettingsManager] Loading settings");

    /*---------------------------------------------------------*\
    | Clear any stored settings before loading                  |
    \*---------------------------------------------------------*/
    settings_data.clear();

    /*---------------------------------------------------------*\
    | Store settings filename, so we can save to it later       |
    \*---------------------------------------------------------*/
    settings_filename = filename;

    /*---------------------------------------------------------*\
    | Add a few logs about the file                             |
    \*---------------------------------------------------------*/
    if(filesystem::exists(settings_filename))
    {
        LOG_INFO("[SettingsManager] File exists (size: %d).", filesystem::file_size(settings_filename));
    }
    else
    {
        LOG_WARNING("[SettingsManager] Configuration file does not exist.");
    }

    /*---------------------------------------------------------*\
    | Open input file in binary mode                            |
    \*---------------------------------------------------------*/
    std::ifstream settings_file(settings_filename, std::ios::in | std::ios::binary);

    /*---------------------------------------------------------*\
    | Read settings into JSON store                             |
    \*---------------------------------------------------------*/
    if(settings_file)
    {
        try
        {
            settings_file >> settings_data;
        }
        catch(const std::exception& e)
        {
            /*-------------------------------------------------*\
            | If an exception was caught, that means the JSON   |
            | parsing failed.  Clear out any data in the store  |
            | as it is corrupt.                                 |
            \*-------------------------------------------------*/
            LOG_ERROR("[SettingsManager] JSON parsing failed: %s", e.what());

            std::stringstream buffer;
            buffer << settings_file.rdbuf();
            LOG_ERROR("[SettingsManager] JSON file: \n ------- \n %s \n ------- \n ", buffer.str().c_str());

            settings_data.clear();
        }
    }

    settings_file.close();
}

void SettingsManager::SaveSettings()
{
    LOG_INFO("[SettingsManager] Saving settings");

    std::ofstream settings_file(settings_filename, std::ios::out | std::ios::binary);

    if(settings_file)
    {
        try
        {
            settings_file << settings_data.dump(4);
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("[SettingsManager] Cannot write to file: %s", e.what());
        }

        settings_file.close();
    }

    /*---------------------------------------------------------*\
    | Add a few logs about the file                             |
    \*---------------------------------------------------------*/
    if(filesystem::exists(settings_filename))
    {
        LOG_INFO("[SettingsManager] File exists (size: %d).", filesystem::file_size(settings_filename));
    }
    else
    {
        LOG_WARNING("[SettingsManager] Configuration file does not exist.");
    }
}
