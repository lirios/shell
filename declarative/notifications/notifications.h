/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <QtCore/QObject>

class QQmlPropertyMap;
class NotificationsDaemon;

class Notifications : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid CONSTANT)
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_ENUMS(CloseReason)
public:
    enum CloseReason {
        CloseReasonExpired = 1,
        CloseReasonByUser,
        CloseReasonByApplication
    };

    Notifications(QObject *parent = 0);

    bool isValid() const;

    bool isActive() const;
    void setActive(bool value);

    NotificationsDaemon *daemon() const;

    Q_INVOKABLE void invokeAction(uint id, const QString &actionId);

    Q_INVOKABLE void closeNotification(uint id, const Notifications::CloseReason &reason);

Q_SIGNALS:
    void activeChanged();

    void notificationReceived(uint notificationId, const QString &appName,
                              const QString &appIcon, bool hasIcon,
                              const QString &summary, const QString &body,
                              const QVariantList &actions, bool isPersistent,
                              int expireTimeout, const QVariantMap &hints);
    void notificationClosed(uint notificationId, uint reason);

    void actionInvoked(uint notificationId, const QString &actionKey);

private:
    bool m_valid;
    bool m_active;
    NotificationsDaemon *m_daemon;
};

#endif // NOTIFICATIONS_H
