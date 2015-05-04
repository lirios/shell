include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/libhawaii
    ${CMAKE_SOURCE_DIR}/libhawaii
)

set(SOURCES
    applicationaction.cpp
    applicationinfo.cpp
    appsmodel.cpp
    appsproxymodel.cpp
    categoriesmodel.cpp
    launcheritem.cpp
    launchermodel.cpp
    menuimageprovider.cpp
    plugin.cpp
    processrunner.cpp
)

add_library(launcherplugin SHARED ${SOURCES})
target_link_libraries(launcherplugin
                      Qt5::DBus
                      Qt5::Xml
                      Qt5::Qml
                      Qt5::Quick
                      GreenIsland::GreenIsland
                      Hawaii::Hawaii)
target_link_libraries(launcherplugin ${QTXDG_LIBRARIES})

install(FILES qmldir plugins.qmltypes
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
install(TARGETS launcherplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
