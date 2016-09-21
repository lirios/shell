include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(SOURCES
    application.cpp
    appsmodel.cpp
    appusage.cpp
    categoriesmodel.cpp
    frequentmodel.cpp
    launchermodel.cpp
    pagemodel.cpp
    plugin.cpp
    processrunner.cpp
    usagetracker.cpp
    utils.cpp
)

add_library(launcherplugin SHARED ${SOURCES})
target_link_libraries(launcherplugin
                      Qt5::DBus
                      Qt5::Xml
                      Qt5::Qml
                      Qt5::Quick
                      Qt5::Sql
                      GreenIsland::Server
                      Hawaii::Core
                      Hawaii::Settings
                      Qt5Xdg)

install(FILES qmldir plugins.qmltypes
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/launcher)
install(TARGETS launcherplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaiios/launcher)
