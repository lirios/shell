include(CheckIncludeFile)
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)

if(FEATURE_enable_systemd)
    find_package(Libsystemd REQUIRED)
endif()

include(LiriSetGitHeadRevision)
liri_set_git_head_revision(_git_rev)

add_executable(LiriShellHelper main.cpp)

file(GLOB_RECURSE _qml_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")
file(GLOB_RECURSE _image_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/images/*")
qt6_add_qml_module(LiriShellHelper
    URI LiriShellHelper
    QML_FILES ${_qml_files}
    RESOURCES ${_image_files}
)

target_compile_definitions(LiriShellHelper PRIVATE GIT_REV="${_git_rev}")

set_target_properties(LiriShellHelper PROPERTIES OUTPUT_NAME "liri-shell-helper")

target_compile_definitions(LiriShellHelper
    PRIVATE
        LIRISHELL_VERSION="${PROJECT_VERSION}"
)

if(LIRI_FEATURE_liri_development_build)
    target_compile_definitions(LiriShellHelper PRIVATE -DDEVELOPMENT_BUILD)
endif()
if(HAVE_SYS_PRCTL_H)
    target_compile_definitions(LiriShellHelper PRIVATE -DHAVE_SYS_PRCTL_H)
endif()
if(FEATURE_shell_enable_systemd)
    target_compile_definitions(LiriShellHelper PRIVATE -DHAVE_SHAVE_SYSTEMD)
    target_link_libraries(LiriShellHelper PRIVATE PkgConfig::Libsystemd)
endif()

target_link_libraries(LiriShellHelper
    PRIVATE
        Qt6::Core
        Qt6::DBus
        Qt6::Gui
        Qt6::GuiPrivate
        Qt6::Qml
        Qt6::Quick
)

install(TARGETS LiriShellHelper
        DESTINATION ${KDE_INSTALL_LIBEXECDIR}
)
