find_package(Wayland REQUIRED)

find_package(QtWaylandScanner REQUIRED)
ecm_add_qtwayland_server_protocol(LiriShellQml_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../../data/protocols/shell-helper.xml"
    BASENAME "shell-helper")

if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()

liri_add_qml_plugin(shell
    MODULE_PATH
        "Liri/Shell"
    SOURCES
        chromeitem.cpp
        chromeitem.h
        hotspot.cpp
        hotspot.h
        keyeventfilter.cpp
        keyeventfilter.h
        logging_p.cpp
        logging_p.h
        plugin.cpp
        shellhelper.cpp
        shellhelper.h
        shellhelper_p.h
        windowmousetracker.cpp
        windowmousetracker.h
        ${LiriShellQml_SOURCES}
    QML_FILES
        Background.qml
        DateTimeIndicator.qml
        DateTime.qml
        Indicator.qml
        LoginGreeter.qml
        PanelItem.qml
        UserDelegate.qml
        qmldir
        plugins.qmltypes
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        INSTALL_ROOTDIR=""
        INSTALL_LIBEXECDIR="${LIBEXECDIR}"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::WaylandCompositor
        Wayland::Server
)
