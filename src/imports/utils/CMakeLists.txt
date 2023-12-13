# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(ShellUtilsQmlPlugin
    URI Liri.Shell.Utils
    VERSION 1.0
    CLASS_NAME ShellUtilsPlugin
    GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(ShellUtilsQmlPlugin
    PRIVATE
        qqmlsortfilterproxymodel.cpp qqmlsortfilterproxymodel.h
)

target_link_libraries(ShellUtilsQmlPlugin
    PRIVATE
        Qt6::Qml
)

ecm_finalize_qml_module(ShellUtilsQmlPlugin)
