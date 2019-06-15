find_package(PipeWire)

if(LIRI_SHELL_WITH_SCREENCAST AND PipeWire_FOUND)
    find_package(GLIB2 REQUIRED)

    set(LiriShellDBusInterfaces_DEFINES SCREENCAST_ENABLED)
    set(LiriShellDBusInterfaces_SOURCES
        screencaststream.cpp
        screencaststream.h
    )
    set(LiriShellDBusInterfaces_LIBRARIES "GLIB2::GLIB2;PipeWire::PipeWire")
else()
    set(LiriShellDBusInterfaces_DEFINES)
    set(LiriShellDBusInterfaces_SOURCES)
    set(LiriShellDBusInterfaces_LIBRARIES)
endif()

if(NOT TARGET Liri::PlatformHeaders)
    find_package(Liri1PlatformHeaders REQUIRED)
endif()

liri_add_module(ShellDBusInterfaces
    SOURCES
        outputchangeset.cpp
        outputchangeset.h
        outputconfigurationadaptor.cpp
        outputconfigurationadaptor.h
        outputconfiguration.cpp
        outputconfiguration.h
        outputdeviceadaptor.cpp
        outputdeviceadaptor.h
        outputdevice.cpp
        outputdevice.h
        outputmanagementadaptor.cpp
        outputmanagementadaptor.h
        outputmanagement.cpp
        outputmanagement.h
        outputsadaptor.cpp
        outputsadaptor.h
        outputs.cpp
        outputs.h
        screencastadaptor.cpp
        screencastadaptor.h
        screencast.cpp
        screencast.h
        screenshooter.h
        screenshoter.cpp
        ${LiriShellDBusInterfaces_SOURCES}
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        QT_WAYLAND_COMPOSITOR_QUICK
        ${LiriShellDBusInterfaces_DEFINES}
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::Qml
        Qt5::Quick
        Qt5::WaylandCompositor
        Liri::PlatformHeaders
        ${LiriShellDBusInterfaces_LIBRARIES}
    COMPILE_FLAGS
        "-fpermissive"
    STATIC
    NO_MODULE_HEADERS
    NO_CMAKE
    NO_PKGCONFIG
)
