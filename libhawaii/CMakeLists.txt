include(GenerateExportHeader)
include(ECMPackageConfigHelpers)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX HAWAII
                  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/hawaii_version.h"
                  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/HawaiiConfigVersion.cmake"
                  SOVERSION ${PROJECT_SOVERSION})

include_directories(
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_BINARY_DIR}
    ${gio_INCLUDE_DIRS}
)

add_definitions(-DQT_NO_KEYWORDS)

set(SOURCES
    qgvariantutils.cpp
    qgsettings.cpp
    utils.cpp
)

add_library(Hawaii SHARED ${SOURCES})
generate_export_header(Hawaii EXPORT_FILE_NAME hawaii/hawaii_export.h)
add_library(Hawaii::Hawaii ALIAS Hawaii)

target_link_libraries(Hawaii
    PUBLIC
        Qt5::Core
    PRIVATE
        ${gio_LIBRARIES}
)

set(HAWAII_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/Hawaii")

target_include_directories(Hawaii INTERFACE "$<INSTALL_INTERFACE:${HAWAII_INCLUDEDIR}>")

set_target_properties(Hawaii PROPERTIES VERSION   ${PROJECT_VERSION}
                                        SOVERSION ${PROJECT_SOVERSION}
                                        EXPORT_NAME Hawaii
)

ecm_generate_headers(Hawaii_CamelCase_HEADERS
  HEADER_NAMES
    QGSettings
  PREFIX
    Hawaii
  REQUIRED_HEADERS Hawaii_HEADERS
)
install(FILES ${Hawaii_CamelCase_HEADERS}
        DESTINATION ${HAWAII_INCLUDEDIR}/Hawaii
        COMPONENT Devel)

install(TARGETS Hawaii EXPORT HawaiiTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/hawaii/hawaii_export.h
      ${Hawaii_HEADERS}
      qgsettings.h
    DESTINATION
      ${GREENISLAND_INCLUDEDIR}/Hawaii
    COMPONENT
      Devel
)

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/Hawaii")

ecm_configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/HawaiiConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/HawaiiConfig.cmake"
                                  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                                  )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/HawaiiConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/HawaiiConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT HawaiiTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE HawaiiTargets.cmake NAMESPACE Hawaii::)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hawaii_version.h
        DESTINATION ${GREENISLAND_INCLUDEDIR}/hawaii COMPONENT Devel)
