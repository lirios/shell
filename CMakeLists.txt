project(Shell)

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
cmake_policy(VERSION 3.4.1)

# Instruct CMake to run moc and rrc automatically when needed
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Apple-specific configuration
set(APPLE_SUPPRESS_X11_WARNING ON)

# Set version
set(PROJECT_VERSION "0.8.90")
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_SOVERSION 0)

if(NOT USE_SUPERBUILD)
    # Set up packaging
    set(CPACK_PACKAGE_NAME "qmlos-shell")
    set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_SET_DESTDIR FALSE)
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_SOURCE_IGNORE_FILES "/build/;/.git;/*.user;/.tx/;~$;${CPACK_SOURCE_IGNORE_FILES}")
    include(CPack)
    add_custom_target(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)
endif()

# ECM
find_package(ECM 1.7.0 REQUIRED NO_MODULE)
list(APPEND CMAKE_MODULE_PATH
    "${ECM_MODULE_PATH}"
    "${ECM_KDE_MODULE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

# Macros
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(CheckIncludeFile)
include(CheckSymbolExists)

# Require at least gcc 4.8
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if ("${CMAKE_CXX_COMPILER_VERSION}" VERSION_LESS "4.8")
        message(SEND_ERROR "Version ${CMAKE_CXX_COMPILER_VERSION} of the ${CMAKE_CXX_COMPILER_ID} C++ compiler is not supported. Please use version 4.8 or later.")
    endif()
endif()

# Adjusting CMAKE_C_FLAGS to get Wayland protocols to compile
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu90")

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Werror -Wall -Wextra -Wformat -Wswitch-default -Wunused-parameter -pedantic -std=c++11")

# qCDebug() uses zero variadic arguments, so we need to disable this warning
# as it would become an error with -Werror
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments")
endif()

# Definitions
#add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY)

# Assume release build by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
endif()

# Options
option(DEVELOPMENT_BUILD "Enable options for developers" OFF)
option(ENABLE_SYSTEMD "Enables systemd support" ON)

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

# Check for prctl
check_include_file("sys/prctl.h" HAVE_SYS_PRCTL_H)
check_symbol_exists(PR_SET_DUMPABLE "sys/prctl.h" HAVE_PR_SET_DUMPABLE)
add_feature_info("prctl-dumpable" HAVE_PR_SET_DUMPABLE "Required to prevent ptrace on the compositor process")

# Find Qt5
set(REQUIRED_QT_VERSION "5.7.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED COMPONENTS
             DBus Xml Widgets Qml Quick QuickControls2 WaylandClient Sql)

# Find Green Island
set(REQUIRED_GREENISLAND_VERSION "0.8.0")
find_package(GreenIsland ${REQUIRED_GREENISLAND_VERSION} REQUIRED COMPONENTS Client Server)

# Find libhawaii
set(REQUIRED_HAWAII_VERSION "0.7.0")
find_package(Hawaii ${REQUIRED_HAWAII_VERSION} REQUIRED)

# Find QtAccountsService
set(REQUIRED_QTACCOUNTSSERVICE "0.6.0")
find_package(QtAccountsService ${REQUIRED_QTACCOUNTSSERVICE_VERSION} REQUIRED)

# lxqt
find_package(QT5XDG REQUIRED)

# Subdirectories
add_subdirectory(3rdparty)
add_subdirectory(client)
add_subdirectory(compositor)
add_subdirectory(data)
add_subdirectory(declarative)
#add_subdirectory(decorations)
add_subdirectory(headers)
add_subdirectory(scripts)
add_subdirectory(tests)

# Install the shell with development builds
if(DEVELOPMENT_BUILD)
    greenisland_install_shell(org.hawaiios.desktop shell)
endif()

# Display featute summary
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
