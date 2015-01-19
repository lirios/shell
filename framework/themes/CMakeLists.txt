include_directories(
    ${CMAKE_BINARY_DIR}/headers
)

add_definitions(-DQT_PLUGIN)

set(SOURCES
    plugin.cpp
    theme.cpp
    themesettings.cpp
    units.cpp
)

add_library(declarative_hawaiithemes SHARED ${SOURCES})
target_link_libraries(declarative_hawaiithemes
    Qt5::Gui
    Qt5::Qml
)

install(TARGETS declarative_hawaiithemes LIBRARY
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Themes)

set(MODULE_FILES
    qmldir
    Palette.qml
    PaletteValues.qml
)
install(FILES ${MODULE_FILES}
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/Themes)
