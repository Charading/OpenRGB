#ifndef FS_H
#define FS_H

#if defined(__has_include) && __has_include(<filesystem>)

#include <filesystem>
namespace fs = std::filesystem;

#else

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#endif // C++17

#endif // FS_H
