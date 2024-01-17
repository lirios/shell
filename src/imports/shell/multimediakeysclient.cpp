// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

MultimediaKeysClient *MultimediaKeysClient::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    return new MultimediaKeysClient();
}
