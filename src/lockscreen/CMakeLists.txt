include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(FEATURE_enable_systemd)
    find_package(Libsystemd REQUIRED)
endif()

find_package(PAM REQUIRED)

include(LiriCreateGitHeadRevisionFile)
liri_create_git_head_revision_file("gitsha1.h")

set(SOURCES
    authenticator.cpp authenticator.h
    extsessionlocksurfacev1.cpp extsessionlocksurfacev1.h
    main.cpp
)
qt6_add_resources(SOURCES lockscreen.qrc)
add_executable(LiriShellLockScreen ${SOURCES})

ecm_qt_declare_logging_category(
    LiriShellLockScreen
    HEADER "lirishelllockscreenlogging.h"
    IDENTIFIER "gLcLockScreen"
    CATEGORY_NAME "liri.shell.lockscreen"
    DEFAULT_SEVERITY "Info"
)

set_target_properties(LiriShellLockScreen PROPERTIES OUTPUT_NAME "liri-shell-lockscreen")

target_compile_definitions(LiriShellLockScreen
    PRIVATE
        LIRISHELL_VERSION="${PROJECT_VERSION}"
)

if(FEATURE_shell_enable_systemd)
    target_compile_definitions(LiriShellLockScreen PRIVATE DEVELOPMENT_BUILD)
endif()
if(HAVE_SYS_PRCTL_H)
    target_compile_definitions(LiriShellLockScreen PRIVATE HAVE_SYS_PRCTL_H)
endif()
if(FEATURE_shell_enable_systemd)
    target_compile_definitions(LiriShellLockScreen PRIVATE HAVE_SHAVE_SYSTEMD)
    target_link_libraries(LiriShellLockScreen PRIVATE PkgConfig::Libsystemd)
endif()

target_link_libraries(LiriShellLockScreen
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::Qml
        ${PAM_LIBRARIES}
)

install(TARGETS LiriShellLockScreen
        DESTINATION ${KDE_INSTALL_LIBEXECDIR}
)
