#include "AutoStart-windows.h"
#include "filesystem.h"

#include <fstream>
#include <iostream>

#include "windows.h"
#include <shlobj.h>

// public methods (Windows Implementation)

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
    bool success = false;
    std::string _autostart_file = GetAutoStartFile();

    if (_autostart_file != "")
    {
        bool weInitialised = false;
        HRESULT result;
        IShellLinkW* shellLink = NULL;

        std::wstring exepathw = utf8_decode(autostart_info.path);
        std::wstring argumentsw = utf8_decode(autostart_info.args);
        std::wstring startupfilepathw = utf8_decode(_autostart_file);
        std::wstring descriptionw = utf8_decode(autostart_info.desc);
        std::wstring iconw = utf8_decode(autostart_info.path);

        result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (void**)&shellLink);
        if (result == CO_E_NOTINITIALIZED)
        {
            weInitialised = true;
            CoInitializeEx(NULL,2u);
            result = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLinkW, (void**)&shellLink);
        }
        if (SUCCEEDED(result)) {
            shellLink->SetPath(exepathw.c_str());
            shellLink->SetArguments(argumentsw.c_str());
            shellLink->SetDescription(descriptionw.c_str());
            shellLink->SetIconLocation(iconw.c_str(), 0);
            IPersistFile* persistFile;
            result = shellLink->QueryInterface(IID_IPersistFile, (void**)&persistFile);
            if (SUCCEEDED(result)) {
                result = persistFile->Save(startupfilepathw.c_str(), TRUE);
                success = SUCCEEDED(result);
                persistFile->Release();
            }
            shellLink->Release();
        }

        if (weInitialised)
        {
            CoUninitialize();
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
    char exepath[MAX_PATH] = "";
    DWORD count = GetModuleFileNameA(NULL, exepath, MAX_PATH);
    return std::string( exepath, (count > 0) ? count : 0 );
}

// private methods (Windows Implementation)
void AutoStart::InitAutoStart(std::string _autostart_name)
{
    char startMenuPath[MAX_PATH];

    autostart_name = _autostart_name;

    HRESULT result = SHGetFolderPathA(NULL, CSIDL_PROGRAMS, NULL, 0, startMenuPath);
    if (SUCCEEDED(result))
    {
        std::string _autostart_file = std::string(startMenuPath);

        _autostart_file += "\\Startup\\" + _autostart_name + ".lnk";
        autostart_file = _autostart_file;
    }
    else
    {
        autostart_file.clear();
    }
}

// Convert an UTF8 string to a wide Unicode String (from wmi.cpp)
std::wstring AutoStart::utf8_decode(const std::string& str)
{
    if (str.empty())
    {
        return std::wstring();
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int) str.size(), nullptr, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int) str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}
