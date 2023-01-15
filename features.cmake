# SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause

#### Features

# liri-development-build
option(FEATURE_liri_development_build "Development build" OFF)
add_feature_info("Liri::DevelopmentBuild" FEATURE_liri_development_build "Build Liri Shell for development")
set(LIRI_FEATURE_liri_development_build "$<IF:${FEATURE_liri_development_build},1,0>")

# liri-enable-systemd
option(FEATURE_enable_systemd "Enable systemd support" ON)
add_feature_info("Liri::Systemd" FEATURE_shell_enable_systemd "Enable systemd support")
set(LIRI_FEATURE_shell_enable_systemd "$<IF:${LIRI_FEATURE_shell_enable_systemd},1,0>")

# shell-enable-qtquick-compiler
option(FEATURE_shell_enable_qtquick_compiler "Build with QtQuick compiler (resulting binaries may not be compatible with future Qt releases without rebuilding them)" OFF)
add_feature_info("Shell::QtQuickCompiler" FEATURE_shell_enable_qtquick_compiler "Build with QtQuick compiler")
set(LIRI_FEATURE_shell_enable_qtquick_compiler "$<IF:${LIRI_FEATURE_shell_enable_qtquick_compiler},1,0>")
