set(SOURCES
    sessionplugin.cpp
    sessioninterface.cpp
)

add_library(sessionplugin SHARED ${SOURCES})
target_link_libraries(sessionplugin Qt5::DBus Qt5::Qml)

install(FILES qmldir
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/session)
install(TARGETS sessionplugin
    DESTINATION ${QML_INSTALL_DIR}/org/hawaii/session)
