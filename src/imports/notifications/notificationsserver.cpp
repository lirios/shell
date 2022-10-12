// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "notificationsadaptor.h"
#include "notificationsserver.h"

Q_GLOBAL_STATIC(NotificationsServer, globalServer)

NotificationsServer::NotificationsServer(QObject *parent)
    : QObject(parent)
    , m_adaptor(new NotificationsAdaptor(this))
{
    qRegisterMetaType<NotificationItem *>("NotificationItem*");

    m_adaptor->registerService();
}

NotificationsServer::~NotificationsServer()
{
    if (m_adaptor) {
        m_adaptor->unregisterService();
        m_adaptor->deleteLater();
        m_adaptor = nullptr;
    }
}

void NotificationsServer::invokeAction(uint id, const QString &actionId)
{
    Q_EMIT m_adaptor->ActionInvoked(id, actionId);
}

void NotificationsServer::closeNotification(uint id)
{
    m_adaptor->CloseNotification(id);
}

NotificationsServer *NotificationsServer::instance()
{
    return globalServer;
}
