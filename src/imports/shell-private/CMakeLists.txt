# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

find_package(Wayland REQUIRED)

if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::AuroraPlatformHeaders)
    find_package(Liri1AuroraPlatformHeaders REQUIRED)
endif()
if(NOT TARGET Liri::AuroraCompositor)
    find_package(Liri1AuroraCompositor REQUIRED)
endif()

ecm_add_qml_module(ShellPrivateQmlPlugin
    URI Liri.private.shell
    VERSION 1.0
    CLASS_NAME ShellPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_compile_definitions(ShellPrivateQmlPlugin PRIVATE LIBEXECDIR="${KDE_INSTALL_FULL_LIBEXECDIR}")

target_sources(ShellPrivateQmlPlugin
    PRIVATE
        dbus/multimediakeysserver.cpp dbus/multimediakeysserver.h
        dbus/osdserver.cpp dbus/osdserver.h
        logging.cpp logging.h
        plugin.cpp
)

target_link_libraries(ShellPrivateQmlPlugin
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::Qml
        Qt6::Quick
        Liri::Xdg
        Liri::AuroraPlatformHeaders
        Liri::AuroraCompositor
)

ecm_finalize_qml_module(ShellPrivateQmlPlugin)
