#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

/*-----------------------------------------*\
|  SettingsManager.h                        |
|                                           |
|  OpenRGB Settings Manager maintains a list|
|  of application settings in JSON format.  |
|  Other components may register settings   |
|  with this class and store/load values.   |
|                                           |
|  Adam Honse (CalcProgrammer1) 11/4/2020   |
\*-----------------------------------------*/

#pragma once

#include "json.hpp"
#include <mutex>

using json = nlohmann::json;

class SettingsManager
{
public:
    SettingsManager();
    ~SettingsManager();

    // Returns a copy of an existing config object
    json    GetSettings(const std::string& settings_key);
    // Returns a direct reference to the requested object; creates one if it does not exist
    json&   GetSettingsRef(const std::string& settings_key);
    void    SetSettings(const std::string& settings_key, const json& new_settings);

    void    LoadSettings(std::string filename);
    void    SaveSettings();

    // Thread-safety: lock to keep the settings consistent when working with a reference
    void    LockSettings();
    void    UnlockSettings();
    
private:
    json        settings_data;
    json        settings_prototype;
    std::string settings_filename;
    std::mutex  settings_mutex;
};

#endif // SETTINGSMANAGER_H
