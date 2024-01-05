# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(ShellQmlPlugin
    URI Liri.Shell
    VERSION 1.0
    CLASS_NAME ShellPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(ShellQmlPlugin
    PRIVATE
        extensionsmodel.cpp extensionsmodel.h
        lirishellv1.cpp lirishellv1.h lirishellv1_p.h
        multimediakeysclient.cpp multimediakeysclient.h
        osdclient.cpp osdclient.h
        plugin.cpp
        wlrlayersurfacev1.cpp wlrlayersurfacev1.h wlrlayersurfacev1_p.h
)

ecm_qt_declare_logging_category(
    ShellQmlPlugin
    HEADER "lirishellclientlogging.h"
    IDENTIFIER "gLcShellClient"
    CATEGORY_NAME "liri.shell.client"
    DEFAULT_SEVERITY "Info"
)

aurora_generate_wayland_protocol_client_sources(ShellQmlPlugin
    FILES
        ${PROJECT_SOURCE_DIR}/data/protocols/liri-shell-unstable-v1.xml
        wlr-layer-shell-unstable-v1.xml
)

ecm_target_qml_sources(ShellQmlPlugin
    SOURCES
        Background.qml
        Indicator.qml
        LoginGreeter.qml
        PanelItem.qml
        StatusAreaExtension.qml
        UserDelegate.qml
)

target_link_libraries(ShellQmlPlugin
    PRIVATE
        Qt6::Core
        Qt6::Concurrent
        Qt6::DBus
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::WaylandClient
        Qt6::Qml
        Qt6::Quick
)

ecm_finalize_qml_module(ShellQmlPlugin)
