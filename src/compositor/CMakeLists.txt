if(NOT TARGET Liri::Qt6GSettings)
    find_package(Qt6GSettings REQUIRED)
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
qt6_add_translation(LiriShell_QM_FILES ${LiriShell_TRANSLATIONS})
install(FILES ${LiriShell_QM_FILES}
        DESTINATION "${KDE_INSTALL_DATADIR}/liri-shell/translations")

set(SOURCES
    application.cpp application.h
    main.cpp
    sessionmanager/sessionmanager.cpp sessionmanager/sessionmanager.h
    ${LiriShell_QM_FILES}
)
add_executable(LiriShell ${SOURCES})

file(GLOB_RECURSE _qml_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.qml")
file(GLOB_RECURSE _js_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/qml/*.js")
file(GLOB_RECURSE _image_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/images/*")
qt6_add_qml_module(LiriShell
    URI compositor
    QML_FILES ${_qml_files} ${_js_files}
    RESOURCES ${_image_files}
)

set_target_properties(LiriShell PROPERTIES OUTPUT_NAME "liri-shell")

target_link_libraries(LiriShell
    PRIVATE
        Qt6::Core
        Qt6::Concurrent
        Qt6::DBus
        Qt6::Gui
        Qt6::Svg
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Liri::Qt6GSettings
        Liri::LocalDevice
        Liri::Xdg
)

target_compile_definitions(LiriShell PRIVATE -DLIRISHELL_VERSION="${PROJECT_VERSION}")
if(FEATURE_shell_enable_systemd)
    target_compile_definitions(LiriShell PRIVATE -DDEVELOPMENT_BUILD)
endif()
if(HAVE_SYS_PRCTL_H)
    target_compile_definitions(LiriShell PRIVATE -DHAVE_SYS_PRCTL_H)
endif()
if(FEATURE_shell_enable_systemd)
    target_compile_definitions(LiriShell PRIVATE -DHAVE_SYSTEMD)
    target_link_libraries(LiriShell PRIVATE PkgConfig::Libsystemd)
endif()

install(TARGETS LiriShell DESTINATION ${KDE_INSTALL_LIBEXECDIR})
