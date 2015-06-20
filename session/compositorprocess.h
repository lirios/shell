/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
    ~CompositorProcess();

    bool isSessionLeader() const;
    void setSessionLeader(bool value);

    QString socketName() const;
    void setSocketName(const QString &name);

    void setProgram(const QString &prog);
    void setArguments(const QStringList &args);

    QProcessEnvironment environment() const;
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
    bool m_sessionLeader;
    SessionLeader *m_process;
    QString m_prog;
    QStringList m_args;
    QProcessEnvironment m_env;

    int m_retries;
    bool m_terminate;
    QString m_xdgRuntimeDir;
    QString m_socketName;
    QFileSystemWatcher *m_watcher;

private Q_SLOTS:
    void processError(QProcess::ProcessError error);
    void processFinished(int exitCode, QProcess::ExitStatus status);
    void socketAvailable();
    bool respawn();
};

#endif // COMPOSITORPROCESS_H
