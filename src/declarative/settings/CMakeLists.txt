include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
)

add_library(hawaiishellsettingsplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellsettingsplugin Qml)
target_link_libraries(hawaiishellsettingsplugin HawaiiCore)

install(TARGETS hawaiishellsettingsplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Settings)

add_subdirectory(qml)
