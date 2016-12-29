include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(SOURCES
    application.cpp
    applicationmanager.cpp
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
                      Qt5::WaylandCompositor
                      Vibe::Core
                      Vibe::Settings
                      Qt5Xdg)

install(FILES qmldir plugins.qmltypes
        DESTINATION ${QML_INSTALL_DIR}/Liri/Launcher)
install(TARGETS launcherplugin
        DESTINATION ${QML_INSTALL_DIR}/Liri/Launcher)
