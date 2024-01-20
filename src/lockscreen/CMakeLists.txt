include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(FEATURE_enable_systemd)
    find_package(Libsystemd REQUIRED)
endif()

find_package(PAM REQUIRED)

include(LiriSetGitHeadRevision)
liri_set_git_head_revision(_git_rev)

set(SOURCES
    authenticator.cpp authenticator.h
    main.cpp
)
add_executable(LiriShellLockScreen ${SOURCES})

file(GLOB_RECURSE _qml_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")
qt6_add_qml_module(LiriShellLockScreen
    URI LiriShellLockscreen
    QML_FILES ${_qml_files}
)

ecm_qt_declare_logging_category(
    LiriShellLockScreen
    HEADER "lirishelllockscreenlogging.h"
    IDENTIFIER "gLcLockScreen"
    CATEGORY_NAME "liri.shell.lockscreen"
    DEFAULT_SEVERITY "Info"
)

target_compile_definitions(LiriShellLockScreen PRIVATE GIT_REV="${_git_rev}")

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
