// SPDX-FileCopyrightText: 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef MULTIMEDIAKEYSSERVER_H
#define MULTIMEDIAKEYSSERVER_H

#include <QObject>

class MultimediaKeysServer : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "io.liri.Shell.MultimediaKeysServer")
public:
    explicit MultimediaKeysServer(QObject *parent = nullptr);

Q_SIGNALS:
    void VolumeMute();
    void VolumeUp();
    void VolumeDown();
    void MediaPlay();
    void MediaPrevious();
    void MediaNext();
};

#endif // MULTIMEDIAKEYSSERVER_H
