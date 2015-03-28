include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    applicationaction.cpp
    applicationinfo.cpp
    appsmodel.cpp
    categoriesmodel.cpp
    launcheritem.cpp
    launchermodel.cpp
    launcherplugin.cpp
)

add_library(launcherplugin SHARED ${SOURCES})
target_link_libraries(launcherplugin
                      Qt5::DBus
                      Qt5::Xml
                      Qt5::Qml
                      Qt5::Quick
                      GreenIsland::GreenIsland)
target_link_libraries(launcherplugin ${QTXDG_LIBRARIES})

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
install(TARGETS launcherplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
