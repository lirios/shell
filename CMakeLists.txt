cmake_minimum_required(VERSION 3.10.0)

project("Shell"
    VERSION "0.9.0"
    DESCRIPTION "QtQuick Wayland compositor and shell"
    LANGUAGES CXX C
)

## Shared macros and functions:
if(NOT LIRI_LOCAL_ECM)
    find_package(LiriCMakeShared "1.0.0" REQUIRED NO_MODULE)
    list(APPEND CMAKE_MODULE_PATH "${LCS_MODULE_PATH}")
endif()

## Add some paths to check for CMake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/cmake/3rdparty")

## Set minimum versions required:
set(QT_MIN_VERSION "5.15.0")

## Liri specific setup common for all modules:
include(LiriSetup)

## Features:
option(LIRI_SHELL_DEVELOPMENT_BUILD "Development build" OFF)
add_feature_info("Shell::DevelopmentBuild" LIRI_SHELL_DEVELOPMENT_BUILD "Build Liri Shell for development")
option(LIRI_SHELL_ENABLE_QTQUICK_COMPILER "Build with QtQuick compiler (resulting binaries may not be compatible with future Qt releases without rebuilding them)" OFF)
add_feature_info("Shell::QtQuickCompiler" LIRI_SHELL_ENABLE_QTQUICK_COMPILER "Build with QtQuick compiler")
option(LIRI_ENABLE_SYSTEMD "Enable systemd support" ON)
add_feature_info("Liri::Systemd" LIRI_ENABLE_SYSTEMD "Enable systemd support")

## Variables:
if(LIRI_ENABLE_SYSTEMD)
    set(INSTALL_SYSTEMDUSERUNITDIR "${INSTALL_LIBDIR}/systemd/user" CACHE PATH "systemd user unit directory [PREFIX/lib/systemd/user]")
endif()

## Find Wayland:
find_package(Wayland 1.15 COMPONENTS Client Server)
set_package_properties(Wayland PROPERTIES TYPE REQUIRED)

## Find Qt 5:
find_package(Qt5 "${QT_MIN_VERSION}"
    CONFIG REQUIRED
    COMPONENTS
        Core
        Concurrent
        DBus
        Xml
        Sql
        Gui
        Widgets
        Svg
        Qml
        Quick
        QuickControls2
        WaylandClient
        WaylandCompositor
        LinguistTools
)

## Add subdirectories:
add_subdirectory(data/settings)
if(LIRI_ENABLE_SYSTEMD)
    add_subdirectory(data/systemd)
endif()
add_subdirectory(src/compositor)
add_subdirectory(src/helper)
add_subdirectory(src/imports/launcher)
add_subdirectory(src/imports/loginmanager)
add_subdirectory(src/imports/mpris2)
add_subdirectory(src/imports/policykit)
add_subdirectory(src/imports/shell)
add_subdirectory(src/imports/shell-private)
add_subdirectory(src/imports/storage)
add_subdirectory(src/plugins/statusarea/datetime)
add_subdirectory(src/plugins/statusarea/user)
#add_subdirectory(src/plugins/statusarea/notifications)
add_subdirectory(src/plugins/statusarea/storage)
