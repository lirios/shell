set(SOURCES
    plugin.cpp
    popupbehavior.cpp
)

add_library(controlsplugin SHARED ${SOURCES})
target_link_libraries(controlsplugin Qt5::Qml Qt5::Quick)

install(TARGETS controlsplugin
    DESTINATION ${QML_INSTALL_DIR}/Hawaii/Controls)

set(FILES
    qmldir
    plugins.qmltypes
    Calendar.qml
    Heading.qml
    TextField.qml
    ToolButton.qml
    SidePanel.qml
)

install(FILES ${FILES}
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Controls)
