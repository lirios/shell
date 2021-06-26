// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OSDCLIENT_H
#define OSDCLIENT_H

#include <QtCore/QObject>

class OsdClient : public QObject
{
    Q_OBJECT
public:
    explicit OsdClient(QObject *parent = nullptr);

    Q_INVOKABLE void showText(const QString &iconName, const QString &text = QString());
    Q_INVOKABLE void showProgress(const QString &iconName, int value);
};

#endif // OSDCLIENT_H
