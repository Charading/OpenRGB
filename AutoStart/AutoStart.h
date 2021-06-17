#pragma once

#include <string>

struct AutoStartInfo
{
    std::string path;
    std::string args;
    std::string desc;
    std::string icon;
    std::string category;
};

class AutoStartInterface
{
public:
    virtual bool DisableAutoStart()                                 = 0;
    virtual bool EnableAutoStart(AutoStartInfo autostart_info)      = 0;
    virtual bool IsAutoStartEnabled()                               = 0;
    virtual std::string GetAutoStartFile()                          = 0;
    virtual std::string GetAutoStartName()                          = 0;
    virtual std::string GetExePath()                                = 0;
};

