/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

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
