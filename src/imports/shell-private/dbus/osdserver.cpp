// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusConnection>

#include "logging.h"
#include "osdserver.h"

OsdServer::OsdServer(QObject *parent)
    : QObject(parent)
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.registerObject(QStringLiteral("/Osd"), this, QDBusConnection::ExportAllSlots))
        qCWarning(lcShell, "Unable to register /Osd D-Bus object");
}

void OsdServer::ShowText(const QString &iconName, const QString &text)
{
    emit textRequested(iconName, text);
}

void OsdServer::ShowProgress(const QString &iconName, int value)
{
    emit progressRequested(iconName, value);
}
