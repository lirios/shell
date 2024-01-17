// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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

OsdClient *OsdClient::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);
    return new OsdClient();
}
