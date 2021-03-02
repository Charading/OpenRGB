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

set(VERSION 0.51)
set(VERSION_FULL ${VERSION}.0.0)
if(MINGW)
    set(CMAKE_SYSTEM_NAME Windows)
    set(CMAKE_SYSTEM_VERSION 6.1) # Windows 7
    set(CMAKE_SIZEOF_VOID_P 8) # For some reason it is not ALWAYS set to anything
endif()


###
# Libraries
###


if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    add_library(hidapi SHARED IMPORTED GLOBAL)
    add_library(usb-1.0 SHARED IMPORTED GLOBAL)
    if(MSVC)
        add_library(inpout32 SHARED IMPORTED GLOBAL)
        if(${CMAKE_SIZEOF_VOID_P} STREQUAL 8)
            set_property(TARGET hidapi PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x64/hidapi.lib")
            set_property(TARGET hidapi PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x64/hidapi.dll")
            set_property(TARGET inpout32 PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/x64/inpoutx64.lib")
            set_property(TARGET inpout32 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/x64/inpoutx64.dll")
            set_property(TARGET usb-1.0 PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS64/dll/libusb-1.0.lib")
            set_property(TARGET usb-1.0 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS64/dll/libusb-1.0.dll")
        else()
            set_property(TARGET hidapi PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.lib")
            set_property(TARGET hidapi PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/hidapi-win/x86/hidapi.dll")
            set_property(TARGET inpout32 PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.lib")
            set_property(TARGET inpout32 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/inpout32_1501/Win32/inpout32.dll")
            set_property(TARGET usb-1.0 PROPERTY IMPORTED_IMPLIB   "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS32/dll/libusb-1.0.lib")
            set_property(TARGET usb-1.0 PROPERTY IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/dependencies/libusb-1.0.22/MS32/dll/libusb-1.0.dll")
        endif()
    endif()
endif()



###
# GIT variables
###

if(WIN32) # Check HOST system to be Windows
    execute_process(COMMAND "date /t" OUTPUT_VARIABLE BUILDDATE)
else()
    execute_process(COMMAND "date -R" OUTPUT_VARIABLE BUILDDATE)
endif()
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse HEAD" OUTPUT_VARIABLE GIT_COMMIT_ID)
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ show -s --format=%ci HEAD" OUTPUT_VARIABLE GIT_COMMENT_DATE)
execute_process(COMMAND "git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$_PRO_FILE_PWD_ rev-parse --abbrev-ref HEAD" OUTPUT_VARIABLE GIT_BRANCH)



###
# Drivers target: A static library containing common major part of both builds
###

add_library(OpenRGB_drivers STATIC)
target_compile_definitions(OpenRGB_drivers PRIVATE OPENRGB_PRIVATE)
target_compile_definitions(OpenRGB_drivers PUBLIC VERSION_STRING="${VERSION}")
target_compile_definitions(OpenRGB_drivers PUBLIC BUILDDATE_STRING="${BUILDDATE}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_COMMIT_ID="${GIT_COMMIT_ID}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_COMMIT_DATE="${GIT_COMMIT_DATE}")
target_compile_definitions(OpenRGB_drivers PUBLIC GIT_BRANCH="${GIT_BRANCH}")
target_include_directories(OpenRGB_drivers PUBLIC "RGBController")
target_include_directories(OpenRGB_drivers PUBLIC "net_port")
target_include_directories(OpenRGB_drivers PUBLIC "i2c_smbus")
target_include_directories(OpenRGB_drivers PUBLIC "pci_ids")
target_include_directories(OpenRGB_drivers PUBLIC "serial_port")
target_include_directories(OpenRGB_drivers PUBLIC "super_io")
target_include_directories(OpenRGB_drivers PUBLIC "wmi")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/json")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/libe131/src")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/NVFC")
target_include_directories(OpenRGB_drivers PUBLIC "dependencies/CRCpp")

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    target_include_directories(OpenRGB_drivers PUBLIC "dependencies/hidapi")
    target_include_directories(OpenRGB_drivers PUBLIC "dependencies/inpout32_1501/Win32")
    target_include_directories(OpenRGB_drivers PUBLIC "dependencies/openrazer-win32/")
    target_include_directories(OpenRGB_drivers PUBLIC "dependencies/display-library/include")
    target_include_directories(OpenRGB_drivers PUBLIC "dependencies/libusb-1.0.22/include")
    file(GLOB src_nvfc dependencies/NVFC/*.cpp)
    file(GLOB src_wmi wmi/*.cpp)
    target_sources(OpenRGB_drivers PRIVATE ${src_nvfc})
    target_sources(OpenRGB_drivers PRIVATE ${src_wmi})
    target_compile_definitions(OpenRGB_drivers PUBLIC USE_HID_USAGE)
    target_compile_definitions(OpenRGB_drivers PUBLIC _MBCS)
    target_compile_definitions(OpenRGB_drivers PUBLIC WIN32)
    target_compile_definitions(OpenRGB_drivers PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions(OpenRGB_drivers PUBLIC _WINSOCK_DEPRECATED_NO_WARNINGS)
    target_compile_definitions(OpenRGB_drivers PUBLIC WIN32_LEAN_AND_MEAN)
    target_compile_definitions(OpenRGB_drivers PUBLIC WINVER=0x0601) # Win7+
    target_compile_definitions(OpenRGB_drivers PUBLIC _WIN32_WINNT=0x0601) # Win7+
endif()
file(GLOB_RECURSE src_drivers Controllers/*.cpp)
file(GLOB src_smbus i2c_smbus/*.cpp)
file(GLOB src_net_port net_port/*.cpp)
file(GLOB src_rgb RGBController/*.cpp)
file(GLOB src_serial serial_port/*.cpp)
file(GLOB src_superio super_io/*.cpp)
file(GLOB src_e131 dependencies/libe131/src/*.cpp)
file(GLOB src_dmiinfo dependencies/*.cpp)
target_sources(OpenRGB_drivers PRIVATE ${src_drivers})
target_sources(OpenRGB_drivers PRIVATE ${src_smbus})
target_sources(OpenRGB_drivers PRIVATE ${src_net_port})
target_sources(OpenRGB_drivers PRIVATE ${src_rgb})
target_sources(OpenRGB_drivers PRIVATE ${src_serial})
target_sources(OpenRGB_drivers PRIVATE ${src_superio})
target_sources(OpenRGB_drivers PRIVATE ${src_e131})
target_sources(OpenRGB_drivers PRIVATE ${src_dmiinfo})

target_link_libraries(OpenRGB_drivers usb-1.0)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    target_link_libraries(OpenRGB_drivers inpout32)
    target_link_libraries(OpenRGB_drivers hidapi)
    # MinGW can't pull some parts of WinAPI by itself, point it in the right direction
    target_link_libraries(OpenRGB_drivers ws2_32)
    target_link_libraries(OpenRGB_drivers ole32)
    target_link_libraries(OpenRGB_drivers oleaut32)
    target_link_libraries(OpenRGB_drivers uuid)
    target_link_libraries(OpenRGB_drivers wbemuuid)
    target_link_libraries(OpenRGB_drivers setupapi)
else()
    target_link_libraries(OpenRGB_drivers pthread)
    target_link_libraries(OpenRGB_drivers hidapi-hidraw)
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
    target_include_directories(OpenRGB_drivers PUBLIC "i2c_tools")
    target_include_directories(OpenRGB_qt PUBLIC "dependencies/ColorWheel")
    file(GLOB_RECURSE src_qt qt/*.ui qt/*.cpp qt/*.qrc)
    file(GLOB_RECURSE src_i2c_tools i2c_tools/*.cpp)
    file(GLOB_RECURSE src_color_wheel dependencies/ColorWheel/*.cpp)

    target_sources(OpenRGB_qt PRIVATE ${src_qt})
    target_sources(OpenRGB_qt PRIVATE ${src_cli}) # Will be recompiled with different parameters
    target_sources(OpenRGB_qt PRIVATE ${src_i2c_tools})
    target_sources(OpenRGB_qt PRIVATE ${src_color_wheel})

    # Add icon
    if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
        set(RC_PATH "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")
        file(WRITE ${RC_PATH} "#include <windows.h>\n\n")
        # icon(s)
        file(GLOB icon_paths qt/*.ico)
        set(icon_no 0)
        foreach(icon_path ${icon_paths})
            math(EXPR icon_no ${icon_no}+1)
            file(TO_NATIVE_PATH "${icon_path}" icon_path_native)
            string(REPLACE \\ \\\\ icon_path_native ${icon_path_native})
            file(APPEND ${RC_PATH} "IDI_ICON${icon_no} ICON DISCARDABLE ${icon_path_native}\n")
        endforeach()
        string(REPLACE . , VERSION_COMMAS ${VERSION_FULL})
        file(APPEND ${RC_PATH} "\nVS_VERSION_INFO VERSIONINFO\n")
        file(APPEND ${RC_PATH} "\tFILEVERSION ${VERSION_COMMAS}\n")
        file(APPEND ${RC_PATH} "\tPRODUCTVERSION ${VERSION_COMMAS}\n")
        file(APPEND ${RC_PATH} "\tBEGIN\n")
        file(APPEND ${RC_PATH} "\t\tBLOCK \"StringFileInfo\"\n\t\tBEGIN\n")
        file(APPEND ${RC_PATH} "\t\t\tBLOCK \"040904b0\"\n\t\t\tBEGIN\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"CompanyName\", \"\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"FileDescription\", \"\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"FileVersion\", \"${VERSION_FULL}\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"LegalCopyright\", \"\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"OriginalFilename\", \"${OUTPUT_NAME}.exe\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"ProductName\", \"${PROJECT_NAME}\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\t\tVALUE \"ProductVersion\", \"${VERSION_FULL}\\0\"\n")
        file(APPEND ${RC_PATH} "\t\t\tEND\n")
        file(APPEND ${RC_PATH} "\t\tEND\n")
        file(APPEND ${RC_PATH} "\tEND\n")
        target_sources(OpenRGB_qt PRIVATE ${RC_PATH})
    elseif(APPLE)
        set(MACOSX_BUNDLE_ICON_FILE OpenRGB.icns)
    endif()

    # Attach drivers and link with Qt
    target_link_libraries(OpenRGB_qt OpenRGB_drivers)
    target_link_libraries(OpenRGB_qt Qt::Core)
    target_link_libraries(OpenRGB_qt Qt::Gui)
    target_link_libraries(OpenRGB_qt Qt::Widgets)

    # Run winqtdeploy on install
    if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
        #install(CODE "execute_process(COMMAND ${WINDEPLOYQT_EXECUTABLE} --verbose 0 --no-compiler-runtime WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)")
    endif()

else()
    message(WARNING "Qt could not be found! The build will only have a CLI executable")
endif()
