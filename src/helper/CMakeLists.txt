include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

set(LiriShellHelper_DEFINES "")
if(LIRI_SHELL_DEVELOPMENT_BUILD)
    list(APPEND LiriShellHelper_DEFINES "DEVELOPMENT_BUILD")
endif()
if(HAVE_SYS_PRCTL_H)
    list(APPEND LiriShellHelper_DEFINES "HAVE_SYS_PRCTL_H")
endif()

find_package(Wayland REQUIRED)

find_package(QtWaylandScanner REQUIRED)
ecm_add_qtwayland_client_protocol(LiriShellHelper_SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/shell-helper.xml"
    BASENAME "shell-helper")

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

if(NOT TARGET Liri::Xdg)
    find_package(Liri1Xdg REQUIRED)
endif()

liri_add_executable(LiriShellHelper
    OUTPUT_NAME
        "liri-shell-helper"
    SOURCES
        main.cpp
        processlauncher.cpp
        processlauncher.h
        shellhelperapplication.cpp
        shellhelperapplication.h
        shellhelperclient.cpp
        shellhelperclient.h
        ${LiriShellHelper_SOURCES}
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIRISHELL_VERSION="${PROJECT_VERSION}"
        ${LiriShellHelper_DEFINES}
    INSTALL_DIRECTORY
        "${INSTALL_LIBEXECDIR}"
    LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::WaylandClient
        Wayland::Client
        Liri::Xdg
)
