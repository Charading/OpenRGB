#include "AutoStart-linux.h"

// public methods (Both Windows and Linux)

std::string AutoStartInterface::GetAutoStartFile()
{
    return (autostart_file);
}

std::string AutoStartInterface::GetAutoStartName()
{
    return (autostart_name);
}
