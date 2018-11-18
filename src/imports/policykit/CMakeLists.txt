find_package(PolkitQt5-1 REQUIRED)

liri_add_qml_plugin(polkit
    MODULE_PATH
        "Liri/PolicyKit"
    SOURCES
        listener_p.cpp
        listener_p.h
        plugin.cpp
        policykitagent.cpp
        policykitagent.h
    QML_FILES
        qmldir
        plugins.qmltypes
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        PolkitQt5-1::Core
        PolkitQt5-1::Agent
)
