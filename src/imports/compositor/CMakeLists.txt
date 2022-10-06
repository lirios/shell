# SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause

liri_add_qml_plugin(LiriShellCompositorQmlPlugin
    MODULE_PATH
        "Liri/Shell/Compositor"
    VERSION
        "1.0"
    OUTPUT_NAME
        lirishellcompositorplugin
    SOURCES
        lirishellcompositorplugin.cpp
        waylandliricolorpickerv1.cpp waylandliricolorpickerv1.h waylandliricolorpickerv1_p.h
        waylandlirimodalv1.cpp waylandlirimodalv1.h waylandlirimodalv1_p.h
        waylandlirishellv1.cpp waylandlirishellv1.h waylandlirishellv1_p.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    LIBRARIES
        Qt::Core
        Qt::Gui
        Liri::AuroraCompositor
)

liri_generate_wayland_protocol_server_sources(LiriShellCompositorQmlPlugin
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/liri-color-picker-unstable-v1.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/liri-shell-unstable-v1.xml
)

liri_finalize_qml_plugin(LiriShellCompositorQmlPlugin)
