// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSADAPTOR_H
#define NOTIFICATIONSADAPTOR_H

#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QLoggingCategory>

class QAtomicInt;
class NotificationItem;
class NotificationsServer;

Q_DECLARE_LOGGING_CATEGORY(NOTIFICATIONS)

class NotificationsAdaptor : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")
public:
    enum CloseReason {
        CloseReasonExpired = 1,
        CloseReasonByUser,
        CloseReasonByApplication
    };
    Q_ENUM(CloseReason)

    explicit NotificationsAdaptor(NotificationsServer *parent);
    ~NotificationsAdaptor();

    bool registerService();
    void unregisterService();

public Q_SLOTS:
    uint Notify(const QString &appName, uint replacesId, const QString &appIcon,
                const QString &summary, const QString &body, const QStringList &actions,
                const QVariantMap &hints, int timeout);

    void CloseNotification(uint id);

    QStringList GetCapabilities();

    QString GetServerInformation(QString &vendor, QString &version, QString &specVersion);

Q_SIGNALS:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &actionKey);

private:
    NotificationsServer *m_server = nullptr;
    QAtomicInt *m_idSeed = nullptr;
    bool m_registered = false;
    QSet<QString> m_spamApplications;
    QHash<QString, uint> m_replaceableNotifications;
    QHash<uint, QString> m_notifications;
    QList<NotificationItem *> m_items;

    uint nextId();
};

#endif // NOTIFICATIONSADAPTOR_H
