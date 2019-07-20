include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(LIRI_ENABLE_SYSTEMD)
    find_package(Libsystemd REQUIRED)
endif()

if(NOT TARGET Liri::WaylandClient)
    find_package(Liri1WaylandClient REQUIRED)
endif()

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

liri_add_executable(LiriShellHelper
    OUTPUT_NAME
        "liri-shell-helper"
    SOURCES
        main.cpp
        shellhelperapplication.cpp
        shellhelperapplication.h
    RESOURCES
        "helper.qrc"
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIRISHELL_VERSION="${PROJECT_VERSION}"
    INSTALL_DIRECTORY
        "${INSTALL_LIBEXECDIR}"
    LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Qml
        Qt5::WaylandClient
        Liri::WaylandClient
)

liri_extend_target("LiriShellHelper"
    CONDITION LIRI_SHELL_DEVELOPMENT_BUILD
    DEFINES DEVELOPMENT_BUILD)

liri_extend_target("LiriShellHelper"
    CONDITION HAVE_SYS_PRCTL_H
    DEFINES HAVE_SYS_PRCTL_H)

liri_extend_target("LiriShellHelper"
    CONDITION LIRI_ENABLE_SYSTEMD
    DEFINES HAVE_SYSTEMD
    LIBRARIES PkgConfig::Libsystemd)
