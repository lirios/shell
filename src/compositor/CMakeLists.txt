include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

find_package(Libsystemd)

find_package(PAM REQUIRED)

if(NOT TARGET Liri::Qt5GSettings)
    find_package(Qt5GSettings REQUIRED)
endif()
if(NOT TARGET Liri::PlatformHeaders)
    find_package(Liri1PlatformHeaders REQUIRED)
endif()
if(NOT TARGET Liri::Logind)
    find_package(Liri1Logind REQUIRED)
endif()
if(NOT TARGET Liri::LocalDevice)
    find_package(Liri1LocalDevice REQUIRED)
endif()
if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()

set(LiriShell_DEFINES "")
set(LiriShell_LIBRARIES "")
if(LIRI_SHELL_DEVELOPMENT_BUILD)
    list(APPEND LiriShell_DEFINES "DEVELOPMENT_BUILD")
endif()
if(HAVE_SYS_PRCTL_H)
    list(APPEND LiriShell_DEFINES "HAVE_SYS_PRCTL_H")
endif()
if(TARGET PkgConfig::Libsystemd)
    list(APPEND LiriShell_DEFINES "HAVE_SYSTEMD")
    list(APPEND LiriShell_LIBRARIES "PkgConfig::Libsystemd")
endif()

find_package(Wayland REQUIRED)
find_package(QtWaylandScanner REQUIRED)

ecm_add_qtwayland_server_protocol(LiriShell_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/liri-decoration.xml"
    BASENAME "liri-decoration")
ecm_add_qtwayland_server_protocol(LiriShell_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/gtk-shell.xml"
    BASENAME "gtk-shell")
ecm_add_qtwayland_server_protocol(LiriShell_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/server-decoration.xml"
    BASENAME "server-decoration")
ecm_add_qtwayland_server_protocol(LiriShell_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/shell-helper.xml"
    BASENAME "shell-helper")

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

if(IS_ABSOLUTE "${INSTALL_LIBEXECDIR}")
    set(LIBEXECDIR "${INSTALL_LIBEXECDIR}")
else()
    set(LIBEXECDIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBEXECDIR}")
endif()

# Translations
file(GLOB LiriShell_TRANSLATIONS "${CMAKE_CURRENT_SOURCE_DIR}/translations/*_*.ts")
qt5_add_translation(LiriShell_QM_FILES ${LiriShell_TRANSLATIONS})
install(FILES ${LiriShell_QM_FILES}
        DESTINATION "${INSTALL_DATADIR}/liri-shell/translations")

liri_add_executable(LiriShell
    OUTPUT_NAME
        "liri-shell"
    SOURCES
        application.cpp
        application.h
        declarative/chromeitem.cpp
        declarative/chromeitem.h
        declarative/fpscounter.cpp
        declarative/fpscounter.h
        declarative/hotspot.cpp
        declarative/hotspot.h
        declarative/indicatorsmodel.cpp
        declarative/indicatorsmodel.h
        declarative/inputsettings.cpp
        declarative/inputsettings.h
        declarative/keyeventfilter.cpp
        declarative/keyeventfilter.h
        declarative/quickoutput.cpp
        declarative/quickoutput.h
        declarative/screenmodel.cpp
        declarative/screenmodel.h
        declarative/shellsurfaceitem.cpp
        declarative/shellsurfaceitem.h
        declarative/windowmousetracker.cpp
        declarative/windowmousetracker.h
        extensions/gtkshell.cpp
        extensions/gtkshell.h
        extensions/gtkshell_p.h
        extensions/kdeserverdecoration.cpp
        extensions/kdeserverdecoration.h
        extensions/liridecoration.cpp
        extensions/liridecoration.h
        extensions/shellhelper.cpp
        extensions/shellhelper.h
        extensions/shellhelper_p.h
        logging.cpp
        logging_p.h
        main.cpp
        multimediakeys/multimediakeys.cpp
        multimediakeys/multimediakeys.h
        onscreendisplay.cpp
        onscreendisplay.h
        processlauncher/io.liri.ProcessLauncher.xml
        processlauncher/processlauncher.cpp
        processlauncher/processlauncher.h
        qmlregistration.cpp
        qmlregistration.h
        sessionmanager/authenticator.cpp
        sessionmanager/authenticator.h
        sessionmanager/qmlauthenticator.cpp
        sessionmanager/qmlauthenticator.h
        sessionmanager/sessionmanager.cpp
        sessionmanager/sessionmanager.h
        sessionmanager/loginmanager/fakebackend.cpp
        sessionmanager/loginmanager/fakebackend.h
        sessionmanager/loginmanager/logindbackend.cpp
        sessionmanager/loginmanager/logindbackend.h
        sessionmanager/loginmanager/loginmanagerbackend.cpp
        sessionmanager/loginmanager/loginmanagerbackend.h
        sessionmanager/loginmanager/loginmanager.cpp
        sessionmanager/loginmanager/loginmanager.h
        sessionmanager/screensaver/screensaver.cpp
        sessionmanager/screensaver/screensaver.h
        ${LiriShell_SOURCES}
        ${LiriShell_QM_FILES}
    RESOURCES
        "compositor.qrc"
    QTQUICK_COMPILER
    DBUS_ADAPTOR_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/processlauncher/io.liri.ProcessLauncher.xml"
        "${CMAKE_CURRENT_SOURCE_DIR}/sessionmanager/screensaver/org.freedesktop.ScreenSaver.xml"
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        QT_WAYLAND_COMPOSITOR_QUICK
        LIRISHELL_VERSION="${PROJECT_VERSION}"
        INSTALL_ROOTDIR=""
        INSTALL_LIBEXECDIR="${LIBEXECDIR}"
        ${LiriShell_DEFINES}
    LIBRARIES
        Qt5::Core
        Qt5::CorePrivate
        Qt5::Concurrent
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Svg
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickControls2
        Qt5::WaylandCompositor
        Wayland::Server
        Liri::Qt5GSettings
        Liri::PlatformHeaders
        Liri::Logind
        Liri::LocalDevice
        Liri::Xdg
        Liri::SigWatch
        Liri::ShellDBusInterfaces
        ${PAM_LIBRARIES}
        ${LiriShell_LIBRARIES}
)
