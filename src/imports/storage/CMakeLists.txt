find_package(KF5Solid REQUIRED)

liri_add_qml_plugin(storage
    MODULE_PATH
        "Liri/Storage"
    VERSION
        "1.0"
    SOURCES
        plugin.cpp
        storagedevice.cpp
        storagedevice.h
        storagemodel.cpp
        storagemodel.h
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
    PUBLIC_LIBRARIES
        Qt5::Core
        Qt5::WaylandCompositor
        KF5::Solid
)
