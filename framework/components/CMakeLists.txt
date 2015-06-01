set(SOURCES
    triangle.cpp
    plugin.cpp
)

add_library(componentsplugin SHARED ${SOURCES})
target_link_libraries(componentsplugin Qt5::Qml Qt5::Quick)

install(TARGETS componentsplugin
    DESTINATION ${QML_INSTALL_DIR}/Hawaii/Components)

set(FILES
    qmldir
    plugins.qmltypes
    Icon.qml
    Loadable.qml
    NoiseBackground.qml
    Object.qml
    Showable.qml
    SmoothFadeImage.qml
    SmoothFadeLoader.qml
)

install(FILES ${FILES}
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Components)

set(LISTITEMS_FILES
    ListItems/Base.qml
    ListItems/Highlight.qml
    ListItems/Subtitled.qml
    ListItems/Standard.qml
    ListItems/ThinSeparator.qml
    ListItems/qmldir
)

install(FILES ${LISTITEMS_FILES}
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Components/ListItems)
