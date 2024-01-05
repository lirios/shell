# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET Liri::AuroraPlatformHeaders)
    find_package(Liri1AuroraPlatformHeaders REQUIRED)
endif()
if(NOT TARGET Liri::AuroraCompositor)
    find_package(Liri1AuroraCompositor REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()

ecm_add_qml_module(LiriShellCompositorQmlPlugin
    URI Liri.Shell.Compositor
    VERSION 1.0
    CLASS_NAME LiriShellCompositorPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_sources(LiriShellCompositorQmlPlugin
    PRIVATE
        abstracttitlebar.cpp abstracttitlebar.h
        appidandicon.cpp appidandicon.h
        chromeitem.cpp chromeitem.h
        desktoplayout.cpp desktoplayout.h
        fpscounter.cpp fpscounter.h
        helperlauncher.cpp helperlauncher.h
        hotspot.cpp hotspot.h
        keyeventfilter.cpp keyeventfilter.h
        lirishellcompositorplugin.cpp
        quickoutput.cpp quickoutput.h
        screencast.cpp screencast.h
        screenmodel.cpp screenmodel.h
        waylandcursorgrabber.cpp waylandcursorgrabber.h
        waylandliricolorpickerv1.cpp waylandliricolorpickerv1.h waylandliricolorpickerv1_p.h
        waylandlirimodalv1.cpp waylandlirimodalv1.h waylandlirimodalv1_p.h
        waylandlirishellv1.cpp waylandlirishellv1.h waylandlirishellv1_p.h
        windowmousetracker.cpp windowmousetracker.h
)

ecm_qt_declare_logging_category(
    LiriShellCompositorQmlPlugin
    HEADER "lirishellcompositorlogging.h"
    IDENTIFIER "lcShellCompositor"
    CATEGORY_NAME "liri.shell.compositor"
    DEFAULT_SEVERITY "Info"
)

aurora_generate_wayland_protocol_server_sources(LiriShellCompositorQmlPlugin
    FILES
        ${PROJECT_SOURCE_DIR}/data/protocols/liri-color-picker-unstable-v1.xml
        ${PROJECT_SOURCE_DIR}/data/protocols/liri-shell-unstable-v1.xml
)

target_compile_definitions(LiriShellCompositorQmlPlugin
	PRIVATE LIBEXECDIR="${KDE_INSTALL_FULL_LIBEXECDIR}"
)

target_link_libraries(LiriShellCompositorQmlPlugin
    PRIVATE
    Qt6::GuiPrivate
        Qt6::Qml
        Qt6::Quick
        Liri::AuroraPlatformHeaders
        Liri::AuroraCompositor
        Liri::Xdg
)

ecm_finalize_qml_module(LiriShellCompositorQmlPlugin)
