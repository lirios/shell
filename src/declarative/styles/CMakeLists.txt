set(SOURCES
    plugin.cpp
    abstractstyle.cpp
    padding.cpp
)

add_library(hawaiishellstylesplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishellstylesplugin Qml)

install(TARGETS hawaiishellstylesplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Styles)

add_subdirectory(qml)
