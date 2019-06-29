find_package(Wayland REQUIRED)

set(AUTOMOC_MOC_OPTIONS -Muri=Liri.private.shell)

if(NOT TARGET Liri::WaylandServer)
    find_package(Liri1WaylandServer REQUIRED)
endif()

liri_add_qml_plugin(shell-private
    MODULE_PATH
        "Liri/private/shell"
    VERSION
        "1.0"
    SOURCES
        chromeitem.cpp
        chromeitem.h
        fpscounter.cpp
        fpscounter.h
        hotspot.cpp
        hotspot.h
        indicatorsmodel.cpp
        indicatorsmodel.h
        inputsettings.cpp
        inputsettings.h
        keyeventfilter.cpp
        keyeventfilter.h
        plugin.cpp
        quickoutput.cpp
        quickoutput.h
        screenmodel.cpp
        screenmodel.h
        shellsurfaceitem.cpp
        shellsurfaceitem.h
        windowmousetracker.cpp
        windowmousetracker.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Qml
        Qt5::Quick
        Qt5::WaylandCompositor
        Liri::ShellDBusInterfaces
        Liri::WaylandServer
    STATIC
)
