liri_add_qml_plugin(ShellQmlPlugin
    MODULE_PATH
        "Liri/Shell"
    VERSION
        "1.0"
    OUTPUT_NAME
        "shellplugin"
    SOURCES
        extensionsmodel.cpp extensionsmodel.h
        lirishellv1.cpp lirishellv1.h lirishellv1_p.h
        multimediakeysclient.cpp multimediakeysclient.h
        osdclient.cpp osdclient.h
        plugin.cpp
        wlrlayersurfacev1.cpp wlrlayersurfacev1.h wlrlayersurfacev1_p.h
    QML_FILES
        Background.qml
        Indicator.qml
        LoginGreeter.qml
        PanelItem.qml
        StatusAreaExtension.qml
        UserDelegate.qml
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Concurrent
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::WaylandClient
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
        liri-shell-unstable-v1.xml
        wlr-layer-shell-unstable-v1.xml
)

liri_finalize_qml_plugin(ShellQmlPlugin)
