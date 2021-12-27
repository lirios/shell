find_package(KF5Solid REQUIRED)

liri_add_qml_plugin(StorageQmlPlugin
    MODULE_PATH
        "Liri/Storage"
    VERSION
        "1.0"
    OUTPUT_NAME
        "storageplugin"
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
        KF5::Solid
)

liri_finalize_qml_plugin(StorageQmlPlugin)
