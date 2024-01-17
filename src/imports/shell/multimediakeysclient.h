// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MULTIMEDIAKEYSCLIENT_H
#define MULTIMEDIAKEYSCLIENT_H

#include <QObject>
#include <QQmlEngine>

class MultimediaKeysClient : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit MultimediaKeysClient(QObject *parent = nullptr);

    static MultimediaKeysClient *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

Q_SIGNALS:
    void volumeMute();
    void volumeUp();
    void volumeDown();
    void mediaPlay();
    void mediaPrevious();
    void mediaNext();
};

#endif // MULTIMEDIAKEYSCLIENT_H
