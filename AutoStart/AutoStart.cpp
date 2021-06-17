#include "AutoStart-linux.h"

// public methods (Both Windows and Linux)

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
