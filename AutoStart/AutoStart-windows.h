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
    std::string GetExePath();

private:
    void InitAutoStart(std::string _autostart_name);
    std::wstring utf8_decode(const std::string& str);
};
