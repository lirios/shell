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

## Set minimum versions required:
set(QT_MIN_VERSION "5.11.0")

## Add some paths to check for CMake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/cmake/3rdparty")

## Liri specific setup common for all modules:
include(LiriSetup)

## Features:
option(LIRI_SHELL_DEVELOPMENT_BUILD "Development build" OFF)
add_feature_info("Shell::DevelopmentBuild" LIRI_SHELL_DEVELOPMENT_BUILD "Build Liri Shell for development")
option(LIRI_SHELL_WITH_SCREENCAST "Build with screencast support" OFF)
add_feature_info("Shell::Pipewire" LIRI_SHELL_WITH_SCREENCAST "Build Liri Shell with screencast support")
option(LIRI_SHELL_ENABLE_QTQUICK_COMPILER "Build with QtQuick compiler (resulting binaries may not be compatible with future Qt releases without rebuilding them)" OFF)
add_feature_info("Shell::QtQuickCompiler" LIRI_SHELL_ENABLE_QTQUICK_COMPILER "Build with QtQuick compiler")

## Variables:
set(INSTALL_SYSTEMDUSERUNITDIR "${INSTALL_LIBDIR}/systemd/user" CACHE PATH "systemd user unit directory [PREFIX/lib/systemd/user]")

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
add_subdirectory(data/systemd)
add_subdirectory(src/compositor)
add_subdirectory(src/helper)
add_subdirectory(src/dbusinterfaces)
add_subdirectory(src/extensions)
add_subdirectory(src/imports/launcher)
add_subdirectory(src/imports/loginmanager)
add_subdirectory(src/imports/mpris2)
add_subdirectory(src/imports/policykit)
add_subdirectory(src/imports/shell)
add_subdirectory(src/imports/shell-private)
add_subdirectory(src/imports/storage)
