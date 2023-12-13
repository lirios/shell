# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(MprisQmlPlugin
    URI Liri.Mpris
    VERSION 1.0
    CLASS_NAME Mpris2Plugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

set_source_files_properties(
    org.mpris.MediaPlayer2.Player.xml
    org.mpris.MediaPlayer2.xml
    PROPERTIES NO_NAMESPACE ON
)

qt_add_dbus_interfaces(_dbus_sources
    org.freedesktop.DBus.Properties.xml
    org.mpris.MediaPlayer2.Player.xml
    org.mpris.MediaPlayer2.xml
)

target_sources(MprisQmlPlugin
    PRIVATE
        mpris2engine.cpp mpris2engine.h
        mpris2player.cpp mpris2player.h
        plugin.cpp
        ${_dbus_sources}
)

target_link_libraries(MprisQmlPlugin
    PRIVATE
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
)

ecm_finalize_qml_module(MprisQmlPlugin)
