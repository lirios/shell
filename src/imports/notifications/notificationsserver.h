// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef NOTIFICATIONSSERVER_H
#define NOTIFICATIONSSERVER_H

#include <QtCore/QObject>
#include <QtCore/QUrl>
#include <QtCore/QVariantMap>

class NotificationsAdaptor;

class NotificationItem
{
public:
    explicit NotificationItem() = default;

    uint notificationId;
    QString appName;
    QString appIcon;
    QUrl iconUrl;
    bool hasIcon;
    QString summary;
    QString body;
    QVariantList actions;
    bool isPersistent;
    int expireTimeout;
    QVariantMap hints;
};

class NotificationsServer : public QObject
{
    Q_OBJECT
public:
    explicit NotificationsServer(QObject *parent = nullptr);
    ~NotificationsServer();

    void invokeAction(uint id, const QString &actionId);
    void closeNotification(uint id);

    static NotificationsServer *instance();

Q_SIGNALS:
    void notificationAdded(NotificationItem *item);
    void notificationReplaced(NotificationItem *item);
    void notificationRemoved(uint id);

private:
    NotificationsAdaptor *m_adaptor = nullptr;
};

#endif // NOTIFICATIONSSERVER_H
