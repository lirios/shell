project(HawaiiShell)

cmake_minimum_required(VERSION 2.8.12)

# Silence CMake warnings
if(POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

# Set version
set(PROJECT_VERSION "0.7.90")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_SOVERSION 0)

# Set up packaging
set(CPACK_PACKAGE_NAME "hawaii-shell")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SET_DESTDIR FALSE)
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)
add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

# Options
option(DEVELOPMENT_BUILD "Enable options for developers" OFF)
option(ENABLE_SYSTEMD "Enables systemd support" ON)
option(ENABLE_ALSA "Enables Alsa mixer backend" ON)
option(ENABLE_PULSEAUDIO "Enables PulseAudio mixer backend" ON)
option(ENABLE_NETWORK_MANAGER "Enables network indicator based on NetworkManager" ON)
option(ENABLE_MODEMMANAGER_SUPPORT "Enables ModemManager support" OFF)

# ECM
find_package(ECM 1.4.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

# Macros
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(CheckIncludeFile)
include(CheckSymbolExists)

# Find pkg-config here so that it can be referenced soon
find_package(PkgConfig)

# systemd user unit directory
if(ENABLE_SYSTEMD)
    if (NOT DEFINED SYSTEMD_USER_UNIT_DIR)
        execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=systemduserunitdir systemd OUTPUT_VARIABLE SYSTEMD_USER_UNIT_DIR)
        string(REGEX REPLACE "[ \t\n]+" "" SYSTEMD_USER_UNIT_DIR ${SYSTEMD_USER_UNIT_DIR})
    endif()
    message(STATUS "systemd user unit directory: ${SYSTEMD_USER_UNIT_DIR}")
endif()

# Require at least gcc 4.8
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8")
        message(SEND_ERROR "Version ${CMAKE_CXX_COMPILER_VERSION} of the ${CMAKE_CXX_COMPILER_ID} C++ compiler is not supported. Please use version 4.8 or later.")
    endif()
endif()

# Adjusting CMAKE_C_FLAGS to get Wayland protocols to compile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -Wextra -Wno-unused-parameter -std=c++11")

# Definitions
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

# Add custom uninstall target
if ("${ECM_VERSION}" VERSION_LESS "1.7.0")
    # ECM 1.7.0 provides this target, so we can't roll our own (see issue #172)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
    add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
endif()

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Check for prctl
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)
check_symbol_exists(PR_SET_DUMPABLE "sys/prctl.h" HAVE_PR_SET_DUMPABLE)
add_feature_info("prctl-dumpable" HAVE_PR_SET_DUMPABLE "Required to prevent ptrace on the compositor process")

# Minimum version requirements
set(QT_MIN_VERSION "5.7.0")
set(KF5_MIN_VERSION "5.1.0")

# Find Qt5
find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
             Core
             DBus
             Xml
             Gui
             Widgets
             Qml
             Quick
             WaylandClient)

# Find KF5
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
             Solid)

# Find NetworkManager-Qt
if(ENABLE_NETWORK_MANAGER)
    find_package(KF5NetworkManagerQt 5.8.0)
    set_package_properties(KF5NetworkManagerQt PROPERTIES
                           TYPE OPTIONAL)
endif()

# Find Green Island
find_package(GreenIsland 0.7.90 REQUIRED COMPONENTS Server)

# Find libhawaii
find_package(Hawaii 0.6.90 REQUIRED)

# lxqt
find_package(QT5XDG REQUIRED)

# Subdirectories
add_subdirectory(3rdparty)
add_subdirectory(compositor)
add_subdirectory(data)
add_subdirectory(declarative)
#add_subdirectory(decorations)
add_subdirectory(framework)
add_subdirectory(headers)
add_subdirectory(scripts)
#add_subdirectory(styles)
add_subdirectory(tests)
add_subdirectory(themes)

# Install the shell with development builds
if(DEVELOPMENT_BUILD)
    greenisland_install_shell(org.hawaiios.desktop shell)
endif()

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
