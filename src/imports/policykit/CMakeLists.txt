find_package(PolkitQt5-1 REQUIRED)

liri_add_qml_plugin(PolkitQmlPlugin
    MODULE_PATH
        "Liri/PolicyKit"
    VERSION
        "1.0"
    OUTPUT_NAME
        "polkitplugin"
    SOURCES
        listener_p.cpp
        listener_p.h
        plugin.cpp
        policykitagent.cpp
        policykitagent.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::DBus
        PolkitQt5-1::Core
        PolkitQt5-1::Agent
)

liri_finalize_qml_plugin(PolkitQmlPlugin)
