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

#include "multimediakeysclient.h"

MultimediaKeysClient::MultimediaKeysClient(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("VolumeMute"),
                this, SIGNAL(volumeMute()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("VolumeUp"),
                this, SIGNAL(volumeUp()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("VolumeDown"),
                this, SIGNAL(volumeDown()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("MediaPlay"),
                this, SIGNAL(mediaPlay()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("MediaPrevious"),
                this, SIGNAL(mediaPrevious()));
    QDBusConnection::sessionBus().connect(
                QStringLiteral("io.liri.Shell"),
                QStringLiteral("/MultimediaKeys"),
                QStringLiteral("io.liri.Shell.MultimediaKeys"),
                QStringLiteral("MediaNext"),
                this, SIGNAL(mediaNext()));
}
