include_directories(
    ${QtConfiguration_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/includes
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/settings
    ${CMAKE_BINARY_DIR}/src/libhawaiishell
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
)

add_library(hawaiishellsettingsplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellsettingsplugin Qml)
target_link_libraries(hawaiishellsettingsplugin HF1HawaiiShell)

install(TARGETS hawaiishellsettingsplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Settings)

add_subdirectory(qml)
