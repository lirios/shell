project(Hawaii)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Options
option(ENABLE_SYSTEMD "Enables systemd support" OFF)
option(ENABLE_ALSA "Enables Alsa mixer backend" ON)
option(ENABLE_PULSEAUDIO "Enables PulseAudio mixer backend" ON)

# Set version
set(HAWAII_VERSION_MAJOR 0)
set(HAWAII_VERSION_MINOR 4)
set(HAWAII_VERSION_PATCH 1)
set(HAWAII_VERSION_STRING "${HAWAII_VERSION_MAJOR}.${HAWAII_VERSION_MINOR}.${HAWAII_VERSION_PATCH}")

# ECM
find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Installation directories
include(KDEInstallDirs)

# Other macros
include(KDECMakeSettings)
include(KDECompilerSettings)

# Macro to print feature summary
include(FeatureSummary)

# Find pkg-config here so that it can be referenced soon
find_package(PkgConfig)

# systemd user unit directory
if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemduserunitdir systemd OUTPUT_VARIABLE SYSTEMD_USER_UNIT_DIR)
    string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_USER_UNIT_DIR ${SYSTEMD_USER_UNIT_DIR})
endif()
message(STATUS "systemd user unit directory: ${SYSTEMD_USER_UNIT_DIR}")

# Require at least gcc 4.8
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8")
        message(SEND_ERROR "Version ${CMAKE_CXX_COMPILER_VERSION} of the ${CMAKE_CXX_COMPILER_ID} C++ compiler is not supported. Please use version 4.8 or later.")
    endif()
endif()

# Adjusting CMAKE_C_FLAGS to get Wayland protocols to compile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")

# Add custom uninstall target
if ("${ECM_VERSION}" VERSION_LESS "1.7.0")
    # ECM 1.7.0 provides this target, so we can't roll our own (see issue #172)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
    add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# Set up packaging
set(CPACK_PACKAGE_NAME "hawaii-shell")
set(CPACK_PACKAGE_VERSION "${HAWAII_VERSION_STRING}")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SET_DESTDIR FALSE)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -Wno-unused-parameter -std=c++11")

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Minimum version requirements
set(QT_MIN_VERSION "5.4.0")
set(KF5_MIN_VERSION "5.1.0")

# Find Qt5
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             Core
             DBus
             Xml
             Gui
             Widgets
             Qml
             Quick)

# Find KF5
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
             Config
             CoreAddons
             Solid)

# Find Green Island
find_package(GreenIsland REQUIRED)

# lxqt
find_package(QT5XDG REQUIRED)

# Wayland
set(REQUIRED_WAYLAND_VERSION 1.2.0)
find_package(Wayland ${REQUIRED_WAYLAND_VERSION} COMPONENTS Client)
set_package_properties(Wayland PROPERTIES
    TYPE REQUIRED
    PURPOSE "Required for protocols")
add_feature_info("Wayland-Client" Wayland_Client_FOUND "Required for protocols")

# Wayland and QtWayland scanner
find_package(QtWaylandScanner REQUIRED)

# Subdirectories
add_subdirectory(3rdparty)
add_subdirectory(compositor)
add_subdirectory(data)
add_subdirectory(declarative)
add_subdirectory(framework)
add_subdirectory(headers)
add_subdirectory(platformtheme)
add_subdirectory(scripts)
add_subdirectory(session)
add_subdirectory(shells)
add_subdirectory(styles)
add_subdirectory(themes)

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
