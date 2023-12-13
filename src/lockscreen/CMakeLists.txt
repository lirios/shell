include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(FEATURE_enable_systemd)
    find_package(Libsystemd REQUIRED)
endif()

find_package(PAM REQUIRED)

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

liri_add_executable(LiriShellLockScreen
    OUTPUT_NAME
        "liri-shell-lockscreen"
    SOURCES
        authenticator.cpp authenticator.h
        extsessionlocksurfacev1.cpp extsessionlocksurfacev1.h
        main.cpp
    RESOURCES
        "lockscreen.qrc"
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
        ${PAM_LIBRARIES}
)

ecm_qt_declare_logging_category(
    LiriShellLockScreen
    HEADER "lirishelllockscreenlogging.h"
    IDENTIFIER "gLcLockScreen"
    CATEGORY_NAME "liri.shell.lockscreen"
    DEFAULT_SEVERITY "Info"
)

liri_extend_target("LiriShellLockScreen"
    CONDITION FEATURE_liri_development_build
    DEFINES DEVELOPMENT_BUILD)

liri_extend_target("LiriShellLockScreen"
    CONDITION HAVE_SYS_PRCTL_H
    DEFINES HAVE_SYS_PRCTL_H)

liri_extend_target("LiriShellLockScreen"
    CONDITION FEATURE_enable_systemd
    DEFINES HAVE_SYSTEMD
    LIBRARIES PkgConfig::Libsystemd)

liri_finalize_executable("LiriShellLockScreen")
