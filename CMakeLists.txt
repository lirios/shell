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
include(GNUInstallDirs)

# Macro to print feature summary
include(FeatureSummary)

# Our CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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
find_package(Qt5 5.1.0 REQUIRED COMPONENTS DBus Qml Quick AccountsService)

# Find GreenIsland
find_package(GreenIsland)
if(GreenIsland_FOUND)
    find_package(Qt5Compositor REQUIRED)
endif()

# Find pkg-config
find_package(PkgConfig "0.22" REQUIRED)

# Find Wayland libraries
include(Wayland)
pkg_check_modules(WaylandClient wayland-client REQUIRED)
pkg_check_modules(WaylandServer wayland-server REQUIRED)

# Find pixman-1 (required for the Weston plugin)
pkg_check_modules(Pixman pixman-1 REQUIRED)

# Find Weston SDK
pkg_check_modules(Weston weston REQUIRED)

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

# KCoreAddons and KConfig
find_package(KCoreAddons REQUIRED)
find_package(KConfig REQUIRED)

# Subdirectories
add_subdirectory(headers)
add_subdirectory(scripts)
add_subdirectory(src)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
