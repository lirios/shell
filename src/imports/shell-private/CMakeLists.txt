find_package(Wayland REQUIRED)

set(AUTOMOC_MOC_OPTIONS -Muri=Liri.private.shell)

if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::WaylandServer)
    find_package(Liri1WaylandServer REQUIRED)
endif()
if(NOT TARGET Liri::PlatformHeaders)
    find_package(Liri1PlatformHeaders REQUIRED)
endif()

liri_add_qml_plugin(shell-private
    MODULE_PATH
        "Liri/private/shell"
    VERSION
        "1.0"
    SOURCES
        dbus/multimediakeysserver.cpp
        dbus/multimediakeysserver.h
        dbus/osdserver.cpp
        dbus/osdserver.h
        chromeitem.cpp
        chromeitem.h
        extensionsmodel.cpp
        extensionsmodel.h
        fpscounter.cpp
        fpscounter.h
        hotspot.cpp
        hotspot.h
        inputsettings.cpp
        inputsettings.h
        keyeventfilter.cpp
        keyeventfilter.h
        logging.cpp
        logging.h
        plugin.cpp
        quickoutput.cpp
        quickoutput.h
        screencast.cpp
        screencast.h
        screenmodel.cpp
        screenmodel.h
        waylandwindow.cpp
        waylandwindow.h
        windowmousetracker.cpp
        windowmousetracker.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Qml
        Qt5::Quick
        Qt5::WaylandCompositor
        Liri::Xdg
        Liri::WaylandServer
        Liri::PlatformHeaders
    STATIC
)
