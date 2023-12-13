# SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause

## Find prctl.h:
include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

## Find Qt:
find_package(Qt6 "6.6.0"
    REQUIRED
    COMPONENTS
        Core
        Concurrent
        DBus
        Xml
        Sql
        Gui
        WaylandClient
        Widgets
        Svg
        Qml
        Quick
        QuickControls2
        LinguistTools
)

## Find Aurora Scanner:
find_package(AuroraScanner REQUIRED)

#### Features

# liri-development-build
option(FEATURE_liri_development_build "Development build" OFF)
add_feature_info("Liri::DevelopmentBuild" FEATURE_liri_development_build "Build Liri Shell for development")
set(LIRI_FEATURE_liri_development_build "$<IF:${FEATURE_liri_development_build},1,0>")

# liri-enable-systemd
option(FEATURE_shell_enable_systemd "Enable systemd support" ON)
if(FEATURE_shell_enable_systemd)
    find_package(Libsystemd QUIET)

    if(NOT TARGET PkgConfig::Libsystemd)
        message(WARNING "You need libsystemd for Shell::Systemd")
        set(FEATURE_enable_systemd OFF)
    endif()
endif()
add_feature_info("Shell::Systemd" FEATURE_shell_enable_systemd "Enable systemd support")
set(LIRI_FEATURE_shell_enable_systemd "$<IF:${LIRI_FEATURE_shell_enable_systemd},1,0>")

## Features summary:
if(NOT LIRI_SUPERBUILD)
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES)
endif()
