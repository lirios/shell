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

## Only build on the appropriate platforms.
if(NOT (UNIX OR NOT ANDROID))
    message(NOTICE "Skipping the build as the condition \"UNIX OR NOT ANDROID\" is not met.")
    return()
endif()

## Features:
include(features.cmake)
if(NOT LIRI_SUPERBUILD)
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()

## Variables:
if(FEATURE_enable_systemd)
    set(INSTALL_SYSTEMDUSERUNITDIR "${INSTALL_LIBDIR}/systemd/user" CACHE PATH "systemd user unit directory [PREFIX/lib/systemd/user]")
endif()

## ECM:
include(ECMQtDeclareLoggingCategory)

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
        LinguistTools
)

## Add subdirectories:
add_subdirectory(data/settings)
if(FEATURE_enable_systemd)
    add_subdirectory(data/systemd)
endif()
add_subdirectory(src/compositor)
add_subdirectory(src/helper)
add_subdirectory(src/imports/compositor)
add_subdirectory(src/imports/loginmanager)
add_subdirectory(src/imports/mpris2)
add_subdirectory(src/imports/notifications)
add_subdirectory(src/imports/policykit)
add_subdirectory(src/imports/shell)
add_subdirectory(src/imports/shell-private)
add_subdirectory(src/imports/storage)
add_subdirectory(src/imports/taskmanager)
add_subdirectory(src/lockscreen)
add_subdirectory(src/plugins/statusarea/datetime)
add_subdirectory(src/plugins/statusarea/user)
#add_subdirectory(src/plugins/statusarea/notifications)
add_subdirectory(src/plugins/statusarea/storage)
add_subdirectory(src/sddm)
