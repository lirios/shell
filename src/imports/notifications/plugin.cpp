// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtQml/QtQml>

#include "notificationsimageprovider.h"
#include "notificationsmodel.h"

class LiriShellNotificationsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        // @uri Liri.Shell.Notifications
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell.Notifications"));

        engine->addImageProvider(QStringLiteral("notifications"),
                                 new NotificationsImageProvider());
    }

    void registerTypes(const char *uri) override
    {
        // @uri Liri.Shell.Notifications
        Q_ASSERT(QLatin1String(uri) == QLatin1String("Liri.Shell.Notifications"));

        qmlRegisterType<NotificationsModel>(uri, 1, 0, "NotificationsModel");
    }
};

#include "plugin.moc"
