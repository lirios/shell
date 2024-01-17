// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef OSDCLIENT_H
#define OSDCLIENT_H

#include <QObject>
#include <QQmlEngine>

class OsdClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit OsdClient(QObject *parent = nullptr);

    Q_INVOKABLE void showText(const QString &iconName, const QString &text = QString());
    Q_INVOKABLE void showProgress(const QString &iconName, int value);

    static OsdClient *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
};

#endif // OSDCLIENT_H
