include_directories(
    "${CMAKE_CURRENT_BINARY_DIR}/../../headers"
    ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)

set(FILES
    plugins.qmltypes
    qmldir
    Background.qml
    DateTime.qml
    DateTimeIndicator.qml
    Indicator.qml
    LoginGreeter.qml
    PanelItem.qml
    UserDelegate.qml
)

set(SOURCES
    logging_p.cpp
    plugin.cpp
    shellhelper.cpp
    windowmousetracker.cpp
)

greenisland_add_server_protocol(SOURCES
    PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../../data/protocols/shell-helper.xml"
    BASENAME shell-helper
)

add_library(shellplugin SHARED ${SOURCES} ${FILES})
target_link_libraries(shellplugin
                      Qt5::Qml
                      Qt5::Quick
                      GreenIsland::Server)

install(FILES ${FILES}
	DESTINATION ${QML_INSTALL_DIR}/Liri/Shell)
install(TARGETS shellplugin
        DESTINATION ${QML_INSTALL_DIR}/Liri/Shell)
