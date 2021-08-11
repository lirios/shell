// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = nullptr);

    Q_INVOKABLE void launchApplication(const QString &appId);
    Q_INVOKABLE void launchDesktopFile(const QString &fileName);
    Q_INVOKABLE void launchCommand(const QString &command);
};

#endif // LAUNCHER_H
