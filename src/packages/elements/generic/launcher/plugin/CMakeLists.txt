include_directories(
    ${QtConfiguration_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/applications
    ${CMAKE_SOURCE_DIR}/src/libhawaiishell/includes
    ${CMAKE_BINARY_DIR}/src/libhawaiishell
    ${CMAKE_BINARY_DIR}/headers
)

set(SOURCES
    plugin.cpp
    launcheritem.cpp
    launchermodel.cpp
)

set(FILES
    qmldir
)

add_library(hawaiishelllauncherplugin SHARED ${SOURCES})
qt5_use_modules(hawaiishelllauncherplugin Qml Quick)
target_link_libraries(hawaiishelllauncherplugin ${QtConfiguration_LIBRARIES})

install(TARGETS hawaiishelllauncherplugin LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Elements/Launcher)
install(FILES ${FILES}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/hawaii/qml/Hawaii/Shell/Elements/Launcher)
