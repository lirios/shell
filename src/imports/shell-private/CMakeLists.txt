if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()

find_package(Wayland REQUIRED)

set(AUTOMOC_MOC_OPTIONS -Muri=Liri.private.shell)

if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()
if(NOT TARGET Liri::AuroraPlatformHeaders)
    find_package(Liri1AuroraPlatformHeaders REQUIRED)
endif()
if(NOT TARGET Liri::AuroraCompositor)
    find_package(Liri1AuroraCompositor REQUIRED)
endif()

liri_add_qml_plugin(ShellPrivateQmlPlugin
    MODULE_PATH
        "Liri/private/shell"
    VERSION
        "1.0"
    OUTPUT_NAME
        "shell-privateplugin"
    SOURCES
        abstracttitlebar.cpp abstracttitlebar.h
        appidandicon.cpp
        appidandicon.h
        dbus/multimediakeysserver.cpp
        dbus/multimediakeysserver.h
        dbus/osdserver.cpp
        dbus/osdserver.h
        chromeitem.cpp
        chromeitem.h
        desktoplayout.cpp desktoplayout.h
        fpscounter.cpp
        fpscounter.h
        helperlauncher.cpp helperlauncher.h
        hotspot.cpp
        hotspot.h
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
        waylandcursorgrabber.cpp
        waylandcursorgrabber.h
        windowmousetracker.cpp
        windowmousetracker.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIBEXECDIR="${LIBEXECDIR}"
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Qml
        Qt5::Quick
        Liri::Xdg
        Liri::AuroraPlatformHeaders
        Liri::AuroraCompositor
    STATIC
)

liri_finalize_qml_plugin(ShellPrivateQmlPlugin)
