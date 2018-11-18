if(NOT TARGET Liri::Qt5AccountsService)
    find_package(Qt5AccountsService REQUIRED)
endif()

liri_add_qml_plugin(loginmanager
    MODULE_PATH
        "Liri/LoginManager"
    SOURCES
        plugin.cpp
        usersmodel.cpp
        usersmodel.h
    QML_FILES
        qmldir
        plugins.qmltypes
    DEFINES
        #QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Liri::Qt5AccountsService
)
