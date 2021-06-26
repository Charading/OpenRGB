#include "AutoStart-linux.h"
#include "filesystem.h"

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <linux/limits.h>


// public methods (Linux Implementation)

AutoStart::AutoStart(std::string _autostart_name)
{
    InitAutoStart(_autostart_name);
}

bool AutoStart::DisableAutoStart()
{
    std::string _autostart_file = GetAutoStartFile();
    std::error_code autostart_file_remove_errcode;
    bool success = false;

    if (_autostart_file != "")
    {
        if (!filesystem::exists(_autostart_file))
        {
            success = true;
        }
        else
        {
            success = filesystem::remove(_autostart_file, autostart_file_remove_errcode);
            if (!success)
            {
                std::cerr << "An error occurred removing the auto start file." << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Could not establish correct autostart file path." << std::endl;
    }

    return success;
}

bool AutoStart::EnableAutoStart(AutoStartInfo autostart_info)
{
    std::string _autostart_file = GetAutoStartFile();
    bool success = false;

    if (_autostart_file != "")
    {
        std::string desktop_file = GenerateDesktopFile(autostart_info);
        std::ofstream autostart_file_stream(_autostart_file, std::ios::out | std::ios::trunc);

        if (!autostart_file_stream)
        {
            std::cerr << "Could not open " << _autostart_file << " for writing." << std::endl;
            success = false;
        }
        else
        {
            autostart_file_stream << desktop_file;
            autostart_file_stream.close();
            success = !autostart_file_stream.fail();
            if (!success)
            {
                std::cerr << "An error occurred writing the auto start file." << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Could not establish correct autostart file path." << std::endl;
    }

    return success;
}

bool AutoStart::IsAutoStartEnabled()
{
    std::string _autostart_file = GetAutoStartFile();

    if (_autostart_file != "")
    {
        return filesystem::exists(_autostart_file);
    }
    return false;
}

std::string AutoStart::GetExePath()
{
    char exepath[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", exepath, PATH_MAX );
    return std::string( exepath, (count > 0) ? count : 0 );
}

// private methods (Linux Implementation)

std::string AutoStart::GenerateDesktopFile(AutoStartInfo autostart_info)
{
    std::stringstream fileContents;
    fileContents << "[Desktop Entry]" << std::endl;
    fileContents << "Categories=" << autostart_info.category << std::endl;
    fileContents << "Comment=" << autostart_info.desc << std::endl;
    fileContents << "Icon=" << autostart_info.icon << std::endl;
    fileContents << "Name=" << GetAutoStartName() << std::endl;
    fileContents << "StartupNotify=true" << std::endl;
    fileContents << "Terminal=false" << std::endl;
    fileContents << "Type=Application" << std::endl;

    fileContents << "Exec=" << autostart_info.path;
    if (autostart_info.args != "")
    {
        fileContents << " " << autostart_info.args;
    }
    fileContents << std::endl;
    return fileContents.str();
}

void AutoStart::InitAutoStart(std::string _autostart_name)
{
    std::string autostart_dir;

    autostart_name = _autostart_name;

    const char *xdg_config_home = getenv("XDG_CONFIG_HOME");
    const char *home = getenv("HOME");

    if (xdg_config_home != NULL)
    {
        autostart_dir = xdg_config_home;
        autostart_dir = autostart_dir + "/autostart/";
    }
    else if (home != NULL)
    {
        autostart_dir = home;
        autostart_dir = autostart_dir + "/.config/autostart/";
    }

    if (autostart_dir != "")
    {
        std::error_code ec;
        bool success = true;
        if (!filesystem::exists(autostart_dir))
        {
            success = filesystem::create_directories(autostart_dir, ec);
        }

        if (success)
        {
            autostart_file = autostart_dir + autostart_name + ".desktop";
        }
    }
}

