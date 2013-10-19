include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
    abstractstyle.cpp
    padding.cpp
    stylesettings.cpp
)

add_library(hawaiishellstylesplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellstylesplugin Qml)
target_link_libraries(hawaiishellstylesplugin HawaiiCore)

install(TARGETS hawaiishellstylesplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles)

add_subdirectory(qml)
