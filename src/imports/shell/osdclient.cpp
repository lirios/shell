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
#include <QDBusMessage>
#include <QDBusPendingCall>

#include "osdclient.h"

OsdClient::OsdClient(QObject *parent)
    : QObject(parent)
{
}

void OsdClient::showText(const QString &iconName, const QString &text)
{
    auto msg = QDBusMessage::createMethodCall(QStringLiteral("io.liri.Shell"),
                                              QStringLiteral("/Osd"),
                                              QStringLiteral("io.liri.Shell.Osd"),
                                              QStringLiteral("ShowText"));
    QVariantList args;
    args.append(iconName);
    args.append(text);
    msg.setArguments(args);
    QDBusConnection::sessionBus().asyncCall(msg);
}

void OsdClient::showProgress(const QString &iconName, int value)
{
    auto msg = QDBusMessage::createMethodCall(QStringLiteral("io.liri.Shell"),
                                              QStringLiteral("/Osd"),
                                              QStringLiteral("io.liri.Shell.Osd"),
                                              QStringLiteral("ShowProgress"));
    QVariantList args;
    args.append(iconName);
    args.append(value);
    msg.setArguments(args);
    QDBusConnection::sessionBus().asyncCall(msg);
}
