// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MULTIMEDIAKEYSCLIENT_H
#define MULTIMEDIAKEYSCLIENT_H

#include <QtCore/QObject>

class MultimediaKeysClient : public QObject
{
    Q_OBJECT
public:
    explicit MultimediaKeysClient(QObject *parent = nullptr);

Q_SIGNALS:
    void volumeMute();
    void volumeUp();
    void volumeDown();
    void mediaPlay();
    void mediaPrevious();
    void mediaNext();
};

#endif // MULTIMEDIAKEYSCLIENT_H
