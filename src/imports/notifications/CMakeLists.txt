liri_add_qml_plugin(NotificationsServerQmlPlugin
    MODULE_PATH
        "Liri/Shell/Notifications"
    VERSION
        "1.0"
    OUTPUT_NAME
        "lirishellnotificationsplugin"
    SOURCES
        notificationsadaptor.cpp notificationsadaptor.h
        notificationsmodel.cpp notificationsmodel.h
        notificationsimageprovider.cpp notificationsimageprovider.h
        notificationsimagesstorage.cpp notificationsimagesstorage.h
        notificationsimageutils.cpp notificationsimageutils.h
        notificationsserver.cpp notificationsserver.h
        plugin.cpp
    QML_FILES
        qmldir
    DEFINES
        QT_NO_CAST_FROM_ASCII
        QT_NO_FOREACH
        PROJECT_VERSION="${PROJECT_VERSION}"
)

liri_finalize_qml_plugin(NotificationsServerQmlPlugin)
