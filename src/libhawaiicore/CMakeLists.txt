include_directories(
    ${QtConfiguration_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/headers
    ${CMAKE_CURRENT_SOURCE_DIR}/settings
)

set(SOURCES
    settings/backgroundsettings.cpp
    settings/launchersettings.cpp
    settings/shellsettings.cpp
)

add_library(HawaiiCore SHARED ${SOURCES})
qt5_use_modules(HawaiiCore Core)
target_link_libraries(HawaiiCore ${QtConfiguration_LIBRARIES})
set_target_properties(HawaiiCore PROPERTIES
    VERSION ${HAWAII_SHELL_VERSION_STRING}
    SOVERSION "${HAWAII_SHELL_VERSION_MAJOR}")

install(TARGETS HawaiiCore
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
