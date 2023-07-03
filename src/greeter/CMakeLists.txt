include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(FEATURE_enable_systemd)
    find_package(Libsystemd REQUIRED)
endif()

if(NOT TARGET Liri::AuroraClient)
    find_package(Liri1AuroraClient REQUIRED)
endif()

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

liri_add_executable(LiriShellGreeter
    OUTPUT_NAME
        "liri-shell-greeter"
    SOURCES
        main.cpp
    RESOURCES
        "greeter.qrc"
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        LIRISHELL_VERSION="${PROJECT_VERSION}"
    INSTALL_DIRECTORY
        "${INSTALL_LIBEXECDIR}"
    LIBRARIES
        Qt5::Core
        Qt5::DBus
        Qt5::Gui
        Qt5::GuiPrivate
        Qt5::Qml
        Liri::AuroraClient
)

liri_extend_target("LiriShellGreeter"
    CONDITION FEATURE_liri_development_build
    DEFINES DEVELOPMENT_BUILD)

liri_extend_target("LiriShellGreeter"
    CONDITION HAVE_SYS_PRCTL_H
    DEFINES HAVE_SYS_PRCTL_H)

liri_extend_target("LiriShellGreeter"
    CONDITION FEATURE_enable_systemd
    DEFINES HAVE_SYSTEMD
    LIBRARIES PkgConfig::Libsystemd)

liri_finalize_executable("LiriShellGreeter")
