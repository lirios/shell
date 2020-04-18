include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(LIRI_ENABLE_SYSTEMD)
    find_package(Libsystemd REQUIRED)
endif()

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

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

# Translations
file(GLOB LiriShell_TRANSLATIONS "${CMAKE_CURRENT_SOURCE_DIR}/../../translations/compositor/*_*.ts")
qt5_add_translation(LiriShell_QM_FILES ${LiriShell_TRANSLATIONS})
install(FILES ${LiriShell_QM_FILES}
        DESTINATION "${INSTALL_DATADIR}/liri-shell/translations")

liri_add_executable(LiriShell
    OUTPUT_NAME
        "liri-shell"
    INSTALL_DIRECTORY
        "${INSTALL_LIBEXECDIR}"
    SOURCES
        application.cpp
        application.h
        main.cpp
        sessionmanager/authenticator.cpp
        sessionmanager/authenticator.h
        sessionmanager/qmlauthenticator.cpp
        sessionmanager/qmlauthenticator.h
        sessionmanager/sessionmanager.cpp
        sessionmanager/sessionmanager.h
        ${LiriShell_QM_FILES}
    RESOURCES
        "compositor.qrc"
    QTQUICK_COMPILER
        ${LIRI_SHELL_ENABLE_QTQUICK_COMPILER}
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIRISHELL_VERSION="${PROJECT_VERSION}"
    LIBRARIES
        Qt5::Core
        Qt5::Concurrent
        Qt5::DBus
        Qt5::Gui
        Qt5::Svg
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickControls2
        Qt5::WaylandCompositor
        Liri::Qt5GSettings
        Liri::PlatformHeaders
        Liri::Logind
        Liri::LocalDevice
        Liri::Xdg
        shell-privateplugin
        ${PAM_LIBRARIES}
)

liri_extend_target("LiriShell"
    CONDITION LIRI_SHELL_DEVELOPMENT_BUILD
    DEFINES DEVELOPMENT_BUILD)

liri_extend_target("LiriShell"
    CONDITION HAVE_SYS_PRCTL_H
    DEFINES HAVE_SYS_PRCTL_H)

liri_extend_target("LiriShell"
    CONDITION LIRI_ENABLE_SYSTEMD
    DEFINES HAVE_SYSTEMD
    LIBRARIES PkgConfig::Libsystemd)
