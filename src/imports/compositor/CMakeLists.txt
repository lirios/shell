# SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
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

liri_add_qml_plugin(LiriShellCompositorQmlPlugin
    MODULE_PATH
        "Liri/Shell/Compositor"
    VERSION
        "1.0"
    OUTPUT_NAME
        lirishellcompositorplugin
    SOURCES
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
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    LIBRARIES
        Qt::Core
        Qt::Gui
        Qt::GuiPrivate
        Liri::AuroraPlatformHeaders
        Liri::AuroraCompositor
        Liri::Xdg
)

if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    liri_extend_target(
        LiriShellCompositorQmlPlugin
        DEFINES LIBEXECDIR="${INSTALL_LIBEXECDIR}"
    )
else()
    liri_extend_target(
        LiriShellCompositorQmlPlugin
        DEFINES LIBEXECDIR="${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}"
    )
endif()

ecm_qt_declare_logging_category(
    LiriShellCompositorQmlPlugin
    HEADER "lirishellcompositorlogging.h"
    IDENTIFIER "lcShellCompositor"
    CATEGORY_NAME "liri.shell.compositor"
    DEFAULT_SEVERITY "Info"
)

liri_generate_wayland_protocol_server_sources(LiriShellCompositorQmlPlugin
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/liri-color-picker-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/protocols/liri-shell-unstable-v1.xml
)

liri_finalize_qml_plugin(LiriShellCompositorQmlPlugin)
