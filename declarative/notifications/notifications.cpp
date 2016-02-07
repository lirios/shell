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

#include "notifications.h"
#include "notificationsdaemon.h"

Notifications::Notifications(QObject *parent)
    : QObject(parent)
    , m_valid(true)
    , m_active(true)
    , m_daemon(new NotificationsDaemon(this))
{
    // Register service
    if (!m_daemon->registerService()) {
        m_valid = false;
        m_active = false;
    }
}

bool Notifications::isValid() const
{
    return m_valid;
}

bool Notifications::isActive() const
{
    return m_active;
}

void Notifications::setActive(bool value)
{
    if (m_active == value)
        return;

    if (value) {
        if (!m_daemon->registerService())
            return;
    } else {
        m_daemon->unregisterService();
    }

    m_active = value;
    Q_EMIT activeChanged();
}

NotificationsDaemon *Notifications::daemon() const
{
    return m_daemon;
}

void Notifications::invokeAction(uint id, const QString &actionId)
{
    Q_EMIT m_daemon->ActionInvoked(id, actionId);
}

void Notifications::closeNotification(uint id, const CloseReason &reason)
{
    if (m_daemon->m_notifications.remove(id) > 0)
        Q_EMIT m_daemon->NotificationClosed(id, (uint)reason);
}

#include "moc_notifications.cpp"
