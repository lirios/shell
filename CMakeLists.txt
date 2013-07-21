project(HawaiiShell)

cmake_minimum_required(VERSION 2.8.9)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Set version
set(HAWAII_VERSION_MAJOR 0)
set(HAWAII_VERSION_MINOR 1)
set(HAWAII_VERSION_PATCH 90)
set(HAWAII_VERSION_STRING "${HAWAII_VERSION_MAJOR}.${HAWAII_VERSION_MINOR}.${HAWAII_VERSION_PATCH}")

# Installation directories
include(HawaiiInstallDirs)

# Log features
include(MacroLogFeature)

# Our CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Include Wayland macros
include(Wayland)

# Add custom uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Set up packaging
set(CPACK_PACKAGE_NAME hawaii-shell)
set(CPACK_PACKAGE_VERSION ${HAWAII_VERSION_STRING})
set(CPACK_SYSTEM_NAME "generic")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SET_DESTDIR FALSE)
include(CPack)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -std=c++0x")

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Disable debug output for release builds
if(CMAKE_BUILD_TYPE MATCHES "^[Rr]elease$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
endif()

# Find Qt5
find_package(Qt5DBus REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Compositor)
find_package(Qt5AccountsService REQUIRED)
macro_log_feature(Qt5DBus_FOUND "Qt5DBus" "Support for Qt5DBus" "http://qt-project.org" "")
macro_log_feature(Qt5Gui_FOUND "Qt5Gui" "Support for Qt5Gui" "http://qt-project.org" "")
macro_log_feature(Qt5Widgets_FOUND "Qt5Widgets" "Support for Qt5Widgets" "http://qt-project.org" "")
macro_log_feature(Qt5Qml_FOUND "Qt5Qml" "Support for Qt5Qml" "http://qt-project.org" "")
macro_log_feature(Qt5Quick_FOUND "Qt5Quick" "Support for Qt5Quick" "http://qt-project.org" "")
macro_log_feature(Qt5Compositor_FOUND "Qt5Compositor" "Support for Qt5Compositor" "http://qt-project.org" "")
macro_log_feature(Qt5AccountsService_FOUND "Qt5AccountsService" "Support for Qt5AccountsService" "http://qt-project.org" "")

# Find GreenIsland
find_package(GreenIsland REQUIRED)
macro_log_feature(GreenIsland_FOUND "GreenIsland" "Green Island SDK" "http://www.maui-project.org" "")

# Find pkg-config
find_package(PkgConfig "0.22" REQUIRED)

# Find Wayland libraries
pkg_check_modules(WaylandClient wayland-client REQUIRED)
pkg_check_modules(WaylandServer wayland-server REQUIRED)
macro_log_feature(WaylandClient_FOUND "WaylandClient" "Support for Wayland clients" "http://wayland.freedesktop.org" "")
macro_log_feature(WaylandServer_FOUND "WaylandServer" "Support for Wayland servers" "http://wayland.freedesktop.org" "")

# Find pixman-1 (required for the Weston plugin)
pkg_check_modules(Pixman pixman-1 REQUIRED)
macro_log_feature(Pixman_FOUND "pixman-1" "Support for pixman" "http://pixman.org" "")

# Find Weston SDK
pkg_check_modules(Weston weston REQUIRED)
macro_log_feature(Weston_FOUND "Weston" "Support for Weston SDK" "http://wayland.freedesktop.org" "")

# Find Weston libdir
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=libdir weston
    OUTPUT_VARIABLE CMAKE_INSTALL_WESTONLIBDIR
    RESULT_VARIABLE _failed)
if(_failed)
    message(FATAL_ERROR "Cannot find libdir propery from weston: ${_failed}")
endif(_failed)
string(REGEX REPLACE "[ \t\n]+" "" CMAKE_INSTALL_WESTONLIBDIR "${CMAKE_INSTALL_WESTONLIBDIR}")

# Find polkit-qt-1
find_package(PolkitQt-1 REQUIRED)
macro_log_feature(PolkitQt-1_FOUND "PolkitQt-1" "Qt bindings for PolicyKit" "http://www.freedesktop.org/wiki/Software/polkit" "")

# KCoreAddons and KConfig
find_package(KCoreAddons REQUIRED)
find_package(KConfig REQUIRED)
macro_log_feature(KCoreAddons_FOUND "KCoreAddons" "KDE Frameworks 5 - KCoreAddons" "http://www.kde.org" "")
macro_log_feature(KConfig_FOUND "KConfig" "KDE Frameworks 5 - KConfig" "http://www.kde.org" "")

# Subdirectories
add_subdirectory(data)
add_subdirectory(headers)
add_subdirectory(scripts)
add_subdirectory(src)

# Display featute log
macro_display_feature_log()
