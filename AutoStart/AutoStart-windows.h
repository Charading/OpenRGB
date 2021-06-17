#pragma once

#include <string>
#include "AutoStart.h"

class AutoStart: public AutoStartInterface
{
public:
    AutoStart(std::string _autostart_name);

    bool DisableAutoStart();
    bool EnableAutoStart(AutoStartInfo autostart_info);
    bool IsAutoStartEnabled();
    std::string GetAutoStartFile();
    std::string GetAutoStartName();
    std::string GetExePath();

private:
    std::string autostart_file;
    std::string autostart_name;

    std::string GenerateDesktopFile(AutoStartInfo autostart_info);
    void InitAutoStart(std::string _autostart_name);
};
