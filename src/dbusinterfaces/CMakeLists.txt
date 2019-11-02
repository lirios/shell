find_package(PipeWire)

if(LIRI_SHELL_WITH_SCREENCAST AND PipeWire_FOUND)
    find_package(GLIB2 REQUIRED)

    set(screencast_SOURCES
        screencaststream.cpp
        screencaststream.h
    )
endif()

if(NOT TARGET Liri::PlatformHeaders)
    find_package(Liri1PlatformHeaders REQUIRED)
endif()

liri_add_module(ShellDBusInterfaces
    SOURCES
        screencastadaptor.cpp
        screencastadaptor.h
        screencast.cpp
        screencast.h
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        QT_WAYLAND_COMPOSITOR_QUICK
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Qml
        Qt5::Quick
        Qt5::WaylandCompositor
        Liri::PlatformHeaders
    COMPILE_FLAGS
        "-fpermissive"
    STATIC
    NO_MODULE_HEADERS
    NO_CMAKE
    NO_PKGCONFIG
)

liri_extend_target("ShellDBusInterfaces"
    CONDITION LIRI_SHELL_WITH_SCREENCAST AND PipeWire_FOUND
    DEFINES SCREENCAST_ENABLED
    SOURCES "${screencast_SOURCES}"
    LIBRARIES "GLIB2::GLIB2;PipeWire::PipeWire")
