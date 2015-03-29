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

#ifndef RESPAWNPROCESS_H
#define RESPAWNPROCESS_H

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QProcessEnvironment>

class RespawnProcess : public QObject
{
    Q_OBJECT
public:
    RespawnProcess(QObject *parent = 0);

public Q_SLOTS:
    void start();
    void stop();
    void terminate();

Q_SIGNALS:
    void started();
    void stopped();
    void finished();

protected:
    QProcess *m_process;
    int m_retries;
    bool m_terminate;

private Q_SLOTS:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus status);
};

#endif // RESPAWNPROCESS_H
