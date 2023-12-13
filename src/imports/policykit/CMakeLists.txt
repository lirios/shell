# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

find_package(PolkitQt6-1 REQUIRED)

ecm_add_qml_module(PolkitQmlPlugin
    URI Liri.PolicyKit
    VERSION 1.0
    CLASS_NAME PolicyKitPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(PolkitQmlPlugin
    PRIVATE
        listener_p.cpp listener_p.h
        plugin.cpp
        policykitagent.cpp policykitagent.h
)

target_link_libraries(PolkitQmlPlugin
    PRIVATE
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
        PolkitQt6-1::Core
        PolkitQt6-1::Agent
)

ecm_finalize_qml_module(PolkitQmlPlugin)
