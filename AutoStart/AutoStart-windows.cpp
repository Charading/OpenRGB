#include "AutoStart-windows.h"
#include "filesystem.h"

#include <windows.h>
#pragma comment(lib, "advapi32")

// public methods (Windows Implementation)

bool AutoStart::DisableAutoStart()
{
    std::string valueName = GetAutoStartName();
    HKEY hkey = NULL;
    LONG openStatus = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE | KEY_QUERY_VALUE, &hkey);
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
    LONG openStatus = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE, &hkey);
    if (!(openStatus==ERROR_SUCCESS))
    {

        return false;
    }
    LONG status = RegSetValueExA(hkey, GetAutoStartName().c_str(), 0, REG_SZ, (BYTE *)exePath.c_str(), (DWORD)(exePath.size()+1));
    return (status == ERROR_SUCCESS);
}

bool AutoStart::IsAutoStartEnabled()
{
    std::string valueName = GetAutoStartName();
    HKEY hkey = NULL;
    LONG openStatus = RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_WRITE | KEY_QUERY_VALUE, &hkey);
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

// private methods (Windows Implementation)
std::string AutoStart::GenerateDesktopFile(AutoStartInfo autostart_info) {
    return "";
}

void AutoStart::InitAutoStart(std::string _autostart_name)
{
    autostart_name = _autostart_name;
    autostart_file = "Registry:HKCU:" + _autostart_name;
}
