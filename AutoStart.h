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
