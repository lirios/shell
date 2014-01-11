include_directories(
    ${QtConfiguration_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/applications
    ${CMAKE_CURRENT_SOURCE_DIR}/settings
    ${CMAKE_SOURCE_DIR}/3rdparty/libqtxdg
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(SOURCES
    ${CMAKE_SOURCE_DIR}/3rdparty/libqtxdg/xdgdesktopfile.cpp
    applications/appcategories.cpp
    applications/appinfo.cpp
    applications/applicationsmodel.cpp
    element.cpp
    hawaiishell.cpp
    pluginmetadata.cpp
    quickview.cpp
    settings/backgroundsettings.cpp
    settings/launchersettings.cpp
    settings/shellsettings.cpp
)

set(HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/hawaiishell_export.h
    applications/appcategories.h
    applications/appinfo.h
    applications/applicationsmodel.h
    hawaiishell.h
    quickview.h
)

add_library(HF1HawaiiShell SHARED ${SOURCES})
add_library(HF1::HawaiiShell ALIAS HF1HawaiiShell)
qt5_use_modules(HF1HawaiiShell Core Gui Quick)
target_link_libraries(HF1HawaiiShell ${QtConfiguration_LIBRARIES})
set_target_properties(HF1HawaiiShell PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    EXPORT_NAME HawaiiShell
)

generate_export_header(HF1HawaiiShell BASE_NAME HawaiiShell)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/HF1HawaiiShellConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/HF1HawaiiShellConfig.cmake
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_PREFIX}/HF1HawaiiShell
    PATH_VARS CMAKE_INSTALL_FULL_INCLUDEDIR CMAKE_INSTALL_PREFIX
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/HF1HawaiiShellConfigVersion.cmake
    VERSION ${HAWAII_SHELL_VERSION_STRING}
    COMPATIBILITY SameMajorVersion
)

install(TARGETS HF1HawaiiShell
    EXPORT HF1HawaiiShellTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/HF1HawaiiShellConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/HF1HawaiiShellConfigVersion.cmake
    HF1HawaiiShellMacros.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HF1HawaiiShell
    COMPONENT Devel
)
install(EXPORT HF1HawaiiShellTargets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/HF1HawaiiShell
    FILE HF1HawaiiShellTargets.cmake
    NAMESPACE HF1::
)
install(FILES ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/HF1/HawaiiShell
    COMPONENT Devel
)
install(DIRECTORY includes/HawaiiShell
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/HF1
    COMPONENT Devel
)
