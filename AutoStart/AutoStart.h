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
    virtual std::string GetExePath()                                = 0;

    std::string GetAutoStartFile();
    std::string GetAutoStartName();

private:
    virtual void InitAutoStart(std::string _autostart_name)         =0;

protected:
    std::string autostart_file;
    std::string autostart_name;
};

