/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#ifndef COMPOSITORPROCESS_H
#define COMPOSITORPROCESS_H

#include <QtCore/QObject>
#include <QtCore/QProcessEnvironment>

#include "sessionleader.h"

class QFileSystemWatcher;

class CompositorProcess : public QObject
{
    Q_OBJECT
public:
    CompositorProcess(bool sessionLeader, QObject *parent = 0);

    bool isSessionLeader() const;
    void setSessionLeader(bool value);

    QString socketName() const;
    void setSocketName(const QString &name);

    void setProgram(const QString &prog);
    void setArguments(const QStringList &args);
    void setEnvironment(const QProcessEnvironment &env);

Q_SIGNALS:
    void started();
    void stopped();
    void finished();

public Q_SLOTS:
    void start();
    void stop();
    void terminate();

private:
    SessionLeader *m_process;
    int m_retries;
    bool m_terminate;
    QString m_xdgRuntimeDir;
    QString m_socketName;
    QFileSystemWatcher *m_watcher;

private Q_SLOTS:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus status);
    void socketAvailable();
};

#endif // COMPOSITORPROCESS_H
