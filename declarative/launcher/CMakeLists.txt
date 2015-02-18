include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    applicationaction.cpp
    applicationinfo.cpp
    applicationmanager.cpp
    applications.cpp
    launcheritem.cpp
    launchermodel.cpp
    launcherplugin.cpp
    window.cpp
    windows.cpp
)

ecm_add_qtwayland_client_protocol(SOURCES
    PROTOCOL ${CMAKE_SOURCE_DIR}/data/protocols/greenisland.xml
    BASENAME greenisland
    PREFIX greenisland_
)

add_library(launcherplugin SHARED ${SOURCES})
target_link_libraries(launcherplugin
                      Qt5::DBus
                      Qt5::Qml
                      Qt5::Quick
                      Wayland::Client
                      KF5::WaylandClient)
if(QTXDG_FOUND)
    target_link_libraries(launcherplugin ${QTXDG_LIBRARIES})
else()
    target_link_libraries(launcherplugin HawaiiQtXDG)
endif()

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
install(TARGETS launcherplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/launcher)
