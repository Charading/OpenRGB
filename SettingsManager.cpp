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

#include <fstream>
#include <iostream>

SettingsManager::SettingsManager()
{

}

SettingsManager::~SettingsManager()
{
    // Make sure that no one is accessing the settings when the manager is being destroyed
    settings_mutex.lock();
    settings_mutex.unlock();
}

json SettingsManager::GetSettings(const std::string& settings_key)
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

json& SettingsManager::GetSettingsRef(const std::string& settings_key)
{
    return settings_data[settings_key];
}

void SettingsManager::SetSettings(const std::string& settings_key, const json& new_settings)
{
    settings_mutex.lock();
    settings_data[settings_key] = new_settings;
    settings_mutex.unlock();
}

void SettingsManager::LoadSettings(std::string filename)
{
    /*---------------------------------------------------------*\
    | Store settings filename, so we can save to it later       |
    \*---------------------------------------------------------*/
    settings_filename = filename;

    /*---------------------------------------------------------*\
    | Open input file in binary mode                            |
    \*---------------------------------------------------------*/
    std::ifstream settings_file(settings_filename, std::ios::in | std::ios::binary);

    /*---------------------------------------------------------*\
    | Read settings into JSON store                             |
    \*---------------------------------------------------------*/
    if(settings_file)
    {
        settings_mutex.lock();
        try
        {
            settings_file >> settings_data;
        }
        catch(std::exception e)
        {
            /*-------------------------------------------------*\
            | If an exception was caught, that means the JSON   |
            | parsing failed.  Clear out any data in the store  |
            | as it is corrupt.                                 |
            \*-------------------------------------------------*/
            settings_data.clear();
        }
        settings_mutex.unlock();
    }

    settings_file.close();
}

void SettingsManager::SaveSettings()
{
    settings_mutex.lock();
    std::ofstream settings_file(settings_filename, std::ios::out | std::ios::binary);

    if(settings_file)
    {
        try
        {
            settings_file << settings_data.dump(4);
        }
        catch(std::exception& e)
        {

        }
        catch(...)
        {

        }

        settings_file.close();
    }
    settings_mutex.unlock();
}

void SettingsManager::LockSettings()
{
    settings_mutex.lock();
}

void SettingsManager::UnlockSettings()
{
    settings_mutex.unlock();
}
