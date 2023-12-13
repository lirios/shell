# SPDX-FileCopyrightText: 2024 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause

ecm_add_qml_module(NotificationsServerQmlPlugin
    URI Liri.Shell.Notifications
    VERSION 1.0
    CLASS_NAME LiriShellNotificationsPlugin
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES QtQuick
)

target_compile_definitions(NotificationsServerQmlPlugin
    PRIVATE PROJECT_VERSION="${PROJECT_VERSION}"
)

target_sources(NotificationsServerQmlPlugin
    PRIVATE
        notificationsadaptor.cpp notificationsadaptor.h
        notificationsmodel.cpp notificationsmodel.h
        notificationsimageprovider.cpp notificationsimageprovider.h
        notificationsimagesstorage.cpp notificationsimagesstorage.h
        notificationsimageutils.cpp notificationsimageutils.h
        notificationsserver.cpp notificationsserver.h
        plugin.cpp
)

target_link_libraries(NotificationsServerQmlPlugin
    PRIVATE
        Qt6::DBus
        Qt6::Qml
        Qt6::Quick
)

ecm_finalize_qml_module(NotificationsServerQmlPlugin)
