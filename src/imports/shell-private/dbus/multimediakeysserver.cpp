// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDBusConnection>

#include "logging.h"
#include "multimediakeysserver.h"

MultimediaKeysServer::MultimediaKeysServer(QObject *parent)
    : QObject(parent)
{
    auto bus = QDBusConnection::sessionBus();
    if (!bus.registerObject(QStringLiteral("/MultimediaKeysServer"), this, QDBusConnection::ExportAllSignals))
        qCWarning(lcShell, "Unable to register /MultimediaKeysServer D-Bus object");
}
