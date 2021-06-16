#include "AutoStart.h"
#include "filesystem.h"

#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#pragma comment(lib, "advapi32")
#endif

#ifdef __unix__
#include <unistd.h>
#include <linux/limits.h>
#endif

// public methods

AutoStart::AutoStart(std::string _autostart_name)
{
    InitAutoStart(_autostart_name);
}

std::string AutoStart::GetAutoStartFile()
{
    return (autostart_file);
}

std::string AutoStart::GetAutoStartName()
{
    return (autostart_name);
}

// public methods (per OS implementations)

#if defined(_WIN32)
bool AutoStart::DisableAutoStart()
{
    std::string valueName = GetAutoStartName();
    HKEY hkey = NULL;
    LONG openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE | KEY_QUERY_VALUE, &hkey);
    if (!(openStatus==ERROR_SUCCESS))
    {
        return false;
    }

    LONG keyExistStatus = RegQueryValueExA(hkey, GetAutoStartName().c_str(),NULL,NULL,NULL,NULL);
    if (keyExistStatus == ERROR_SUCCESS)
    {
        LONG status = RegDeleteValueA(hkey, GetAutoStartName().c_str());
        return (status == ERROR_SUCCESS);
    }
    else
    {
        return true;
    }

}

bool AutoStart::EnableAutoStart(AutoStartInfo autostart_info)
{
    std::string valueName = GetAutoStartName();
    std::string exePath = "\"" + autostart_info.path + "\"";
    if (autostart_info.args != "")
    {
        exePath = exePath + " " + autostart_info.args;
    }

    HKEY hkey = NULL;
    LONG openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey);
    if (!(openStatus==ERROR_SUCCESS))
    {

        return false;
    }
    LONG status = RegSetValueExA(hkey, GetAutoStartName().c_str(), 0, REG_SZ, (BYTE *)exePath.c_str(), (exePath.size()+1) * sizeof(char));
    return (status == ERROR_SUCCESS);
}

bool AutoStart::IsAutoStartEnabled()
{
    std::string valueName = GetAutoStartName();
    HKEY hkey = NULL;
    LONG openStatus = RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE | KEY_QUERY_VALUE, &hkey);
    if (!(openStatus==ERROR_SUCCESS))
    {
        return false;
    }

    LONG keyExistStatus = RegQueryValueExA(hkey, GetAutoStartName().c_str(),NULL,NULL,NULL,NULL);
    return (keyExistStatus == ERROR_SUCCESS);
}

std::string AutoStart::GetExePath()
{
    char exepath[MAX_PATH] = "";
    DWORD count = GetModuleFileNameA(NULL, exepath, MAX_PATH);
    return std::string( exepath, (count > 0) ? count : 0 );
}
#elif defined(__unix__)
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
#endif

// private methods (per OS implementations)

#if defined(_WIN32)
std::string AutoStart::GenerateDesktopFile(AutoStartInfo autostart_info) {
    return "";
}

void AutoStart::InitAutoStart(std::string _autostart_name)
{
    autostart_name = _autostart_name;
    autostart_file = "Registry:HKCU:" + _autostart_name;
}
#elif defined(__unix__)
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
#endif
