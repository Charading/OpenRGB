cmake_minimum_required(VERSION 3.5)

project(OpenRGB LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt5 COMPONENTS Core OPTIONAL_COMPONENTS)
find_package(Qt5 COMPONENTS Gui OPTIONAL_COMPONENTS)
find_package(Qt5 COMPONENTS Widgets OPTIONAL_COMPONENTS)



###
# Libraries
###

add_library(hidapi SHARED IMPORTED GLOBAL)
add_library(inpout SHARED IMPORTED GLOBAL)
add_library(libusb SHARED IMPORTED GLOBAL)

if(WIN32)
    if(MINGW)
        set_property(TARGET hidapi PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.lib")
        set_property(TARGET hidapi PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.dll")
        # inpout needs to be rebuilt to link properly
        set_property(TARGET inpout PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.lib")
        set_property(TARGET inpout PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.dll")
        set_property(TARGET libusb PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MinGW32/dll/libusb-1.0.dll.a")
        set_property(TARGET libusb PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MinGW32/dll/libusb-1.0.dll")
    elseif(MSVC)
        if(CMAKE_CL_64)
            set_property(TARGET hidapi PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x64/hidapi.lib")
            set_property(TARGET hidapi PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x64/hidapi.dll")
            set_property(TARGET inpout PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/x64/inpoutx64.lib")
            set_property(TARGET inpout PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/x64/inpoutx64.dll")
            set_property(TARGET libusb PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS64/dll/libusb-1.0.lib")
            set_property(TARGET libusb PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS64/dll/libusb-1.0.dll")
        else()
            set_property(TARGET hidapi PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.lib")
            set_property(TARGET hidapi PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.dll")
            set_property(TARGET inpout PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.lib")
            set_property(TARGET inpout PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.dll")
            set_property(TARGET libusb PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS32/dll/libusb-1.0.lib")
            set_property(TARGET libusb PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS32/dll/libusb-1.0.dll")
        endif()
    endif()
endif()



###
# GIT variables
###

if(WIN32)
    execute_process(COMMAND "date /t" OUTPUT_VARIABLE BUILDDATE)
else()
    execute_process(COMMAND "date -R -d \"@${SOURCE_DATE_EPOCH:-$(date +%s)}\"" OUTPUT_VARIABLE BUILDDATE)
endif()
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD" OUTPUT_VARIABLE GIT_COMMIT_ID)
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ show -s --format=%ci HEAD" OUTPUT_VARIABLE GIT_COMMENT_DATE)
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD" OUTPUT_VARIABLE GIT_BRANCH)



###
# Drivers target: A static library containing common major part of both builds
###

add_library(OpenRGB_drivers STATIC)
target_compile_definitions(OpenRGB_drivers PRIVATE OPENRGB_PRIVATE)
target_compile_definitions(OpenRGB_drivers PUBLIC VERSION_STRING="${VERSION_STRING}")
target_compile_definitions(OpenRGB_drivers PUBLIC BUILDDATE_STRING="${BUILDDATE}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_COMMIT_ID="${GIT_COMMIT_ID}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_COMMIT_DATE="${GIT_COMMIT_DATE}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_BRANCH="${GIT_BRANCH}")
target_include_directories(OpenRGB_drivers PUBLIC "RGBController")
target_include_directories(OpenRGB_drivers PUBLIC "net_port")
target_include_directories(OpenRGB_drivers PUBLIC "i2c_smbus")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/json")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/hidapi")
if(WIN32)
    file(GLOB_RECURSE src_nvfc RELATIVE "dependencies/NVFC" "*.cpp")
    file(GLOB_RECURSE src_wmi RELATIVE "wmi/" "*.cpp")
    target_compile_definitions(OpenRGB_drivers PUBLIC USE_HID_USAGE)
    target_compile_definitions(OpenRGB_drivers PUBLIC _MBCS)
    target_compile_definitions(OpenRGB_drivers PUBLIC WIN32)
    target_compile_definitions(OpenRGB_drivers PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(OpenRGB_drivers PUBLIC _WINSOCK_DEPRECATED_NO_WARNINGS)
    target_compile_definitions(OpenRGB_drivers PUBLIC WIN32_LEAN_AND_MEAN)
    target_compile_definitions(OpenRGB_drivers PUBLIC WINVER=0x0601) # Win7+
    target_compile_definitions(OpenRGB_drivers PUBLIC _WIN32_WINNT=0x0601) # Win7+
endif()
file(GLOB_RECURSE src_drivers RELATIVE "Controllers/" "*.cpp")
file(GLOB_RECURSE src_smbus RELATIVE "i2c_smbus/" "*.cpp")
file(GLOB_RECURSE src_net_port RELATIVE "net_port/" "*.cpp")
file(GLOB_RECURSE src_net_pci RELATIVE "pci_ids/" "*.cpp")
file(GLOB_RECURSE src_rgb RELATIVE "RGBController/" "*.cpp")
file(GLOB_RECURSE src_serial RELATIVE "serial_port/" "*.cpp")
file(GLOB_RECURSE src_e131 RELATIVE "dependencies/libe131" "*.cpp")
target_sources(OpenRGB_drivers PRIVATE ${src_drivers})
target_sources(OpenRGB_drivers PRIVATE ${src_smbus})
target_sources(OpenRGB_drivers PRIVATE ${src_net_port})
target_sources(OpenRGB_drivers PRIVATE ${src_net_pci})
target_sources(OpenRGB_drivers PRIVATE ${src_rgb})
target_sources(OpenRGB_drivers PRIVATE ${src_serial})
target_sources(OpenRGB_drivers PRIVATE ${src_e131})
target_sources(OpenRGB_drivers PRIVATE ${src_nvfc})
target_sources(OpenRGB_drivers PRIVATE ${src_wmi})
target_link_libraries(OpenRGB_drivers hidapi)
target_link_libraries(OpenRGB_drivers libusb)
target_link_libraries(OpenRGB_drivers inpout)
# MinGW can't pull some parts of WinAPI by itself, point it in the right direction
if(MINGW)
    target_link_options(OpenRGB_drivers PUBLIC "-lws2_32 -lole32 -mwindows")
endif()


###
# CLI target (no GUI at all, no CLI issues on Windows, does not rely on Qt)
###

add_executable(OpenRGB_cli)
target_compile_definitions(OpenRGB_cli PRIVATE OPENRGB_PRIVATE)
target_compile_definitions(OpenRGB_cli PUBLIC OPENRGB_CLI)
# Pull in sources
file(GLOB src_cli "*.cpp")
target_sources(OpenRGB_cli PRIVATE ${src_cli})
# Attach drivers
target_link_libraries(OpenRGB_cli OpenRGB_drivers)



###
# GUI target (standard Qt build, no CLI output on Windows)
# Uses all files of the CLI build, but with an additional define for Qt GUI (OPENRGB_QT)
# The define is NOT used in the driver/dependency
# The target is ignored if Qt is not installed, still allowing a CLI build
###

if(Qt5Core_FOUND AND Qt5Gui_FOUND AND Qt5Widgets_FOUND)
    add_executable(OpenRGB_qt)
    target_compile_definitions(OpenRGB_qt PRIVATE OPENRGB_PRIVATE)
    target_compile_definitions(OpenRGB_qt PUBLIC OPENRGB_QT)
    # Pull in sources
    target_include_directories(OpenRGB_qt PUBLIC "qt")
    target_include_directories(OpenRGB_qt PUBLIC "dependencies/ColorWheel")
    file(GLOB_RECURSE src_qt RELATIVE "qt/" "*.cpp")
    file(GLOB_RECURSE src_i2c_tools RELATIVE "i2c_tools/" "*.cpp" "*.qrc" "*.ui")
    file(GLOB_RECURSE src_color_wheel RELATIVE "dependencies/ColorWheel" "*.cpp")

    # TODO: make auto-generated
    target_sources(OpenRGB_qt PRIVATE ${src_cli}) # Will be recompiled with different parameters
    target_sources(OpenRGB_qt PRIVATE ${src_qt})
    target_sources(OpenRGB_qt PRIVATE ${src_i2c_tools})
    target_sources(OpenRGB_qt PRIVATE ${src_color_wheel})
    target_sources(OpenRGB_qt PRIVATE "qt/resources.rc") # adds the icon; may need to be changed to something better
    # Attach drivers and link with Qt
    target_link_libraries(OpenRGB_qt OpenRGB_drivers)
    target_link_libraries(OpenRGB_qt Core)
    target_link_libraries(OpenRGB_qt Gui)
    target_link_libraries(OpenRGB_qt Widgets)
else()
    message(WARNING "Qt could not be found! The build will only have a CLI executable")
endif()

