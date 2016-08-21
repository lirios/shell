set(SOURCES
    plugin.cpp
    usersmodel.cpp
)

add_library(loginmanagerplugin SHARED ${SOURCES})
target_link_libraries(loginmanagerplugin
                      Qt5::Qml
                      Qt5::Quick
                      QtAccountsService::QtAccountsService)

install(FILES qmldir plugins.qmltypes
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/LoginManager)
install(TARGETS loginmanagerplugin
        DESTINATION ${QML_INSTALL_DIR}/Hawaii/LoginManager)
