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

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QTimer>

#include "compositorlauncher.h"
#include "compositorprocess.h"

#include <signal.h>

CompositorProcess::CompositorProcess(bool sessionLeader, QObject *parent)
    : QObject(parent)
    , m_sessionLeader(sessionLeader)
    , m_process(Q_NULLPTR)
    , m_retries(5)
    , m_terminate(false)
    , m_xdgRuntimeDir(QString::fromUtf8(qgetenv("XDG_RUNTIME_DIR")))
    , m_watcher()
{
}

CompositorProcess::~CompositorProcess()
{
    stop();
}

bool CompositorProcess::isSessionLeader() const
{
    return m_sessionLeader;
}

void CompositorProcess::setSessionLeader(bool value)
{
    m_sessionLeader = value;
}

QString CompositorProcess::socketName() const
{
    return m_socketName;
}

void CompositorProcess::setSocketName(const QString &name)
{
    m_socketName = name;
}

void CompositorProcess::setProgram(const QString &prog)
{
    m_prog = prog;
}

void CompositorProcess::setArguments(const QStringList &args)
{
    m_args = args;
}

QProcessEnvironment CompositorProcess::environment() const
{
    return m_env;
}

void CompositorProcess::setEnvironment(const QProcessEnvironment &env)
{
    m_env = env;
}

void CompositorProcess::start()
{
    qCDebug(COMPOSITOR)
            << "Starting:"
            << qPrintable(m_prog)
            << qPrintable(m_args.join(' '));
    if (m_process) {
        qCWarning(COMPOSITOR) << "Process already running";
        return;
    }

    m_process = new SessionLeader(m_sessionLeader);
    m_process->setProgram(m_prog);
    m_process->setArguments(m_args);
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);
    m_process->setProcessEnvironment(m_env);
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processFinished(int,QProcess::ExitStatus)));

    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(socketAvailable()));
    m_watcher->addPath(m_xdgRuntimeDir);

    m_process->start();
}

void CompositorProcess::stop()
{
    if (!m_process)
        return;

    qCDebug(COMPOSITOR)
            << "Stopping:"
            << qPrintable(m_process->program())
            << qPrintable(m_process->arguments().join(' '));

    disconnect(m_watcher, SIGNAL(directoryChanged(QString)),
               this, SLOT(socketAvailable()));
    m_watcher->deleteLater();
    m_watcher = Q_NULLPTR;

    m_process->terminate();
    if (!m_process->waitForFinished())
        m_process->kill();
    m_process->deleteLater();
    m_process = Q_NULLPTR;
    Q_EMIT stopped();
}

void CompositorProcess::terminate()
{
    if (!m_process)
        return;

    qCDebug(COMPOSITOR)
            << "Terminate:"
            << qPrintable(m_process->program())
            << qPrintable(m_process->arguments().join(' '));

    disconnect(m_watcher, SIGNAL(directoryChanged(QString)),
               this, SLOT(socketAvailable()));
    m_watcher->deleteLater();
    m_watcher = Q_NULLPTR;

    m_terminate = true;
    ::kill(m_process->processId(), SIGINT);
    m_process->waitForFinished();
    m_process->deleteLater();
    m_process = Q_NULLPTR;
}

void CompositorProcess::processError(QProcess::ProcessError error)
{
    Q_UNUSED(error)

    if (m_terminate) {
        m_terminate = false;
        return;
    }

    qCWarning(COMPOSITOR)
            << "Process" << m_process->program()
            << "had an error:" << m_process->errorString();

    // Respawn process
    if (respawn())
        return;

    // Compositor failed to start, kill full screen shell and terminate
    qCritical(COMPOSITOR)
            << "Process" << m_prog
            << "won't start, aborting...";
    QCoreApplication::quit();
}

void CompositorProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
    if (exitCode == 0) {
        qCWarning(COMPOSITOR)
                << "Process" << m_prog
                << "finished successfully";
        Q_EMIT finished();
        m_process->deleteLater();
        m_process = Q_NULLPTR;
    } else if (status == QProcess::NormalExit) {
        qCWarning(COMPOSITOR)
                << "Process" << m_prog
                << "finished with exit code" << exitCode;
        Q_EMIT stopped();
        m_process->deleteLater();
        m_process = Q_NULLPTR;
    }
}

void CompositorProcess::socketAvailable()
{
    if (m_socketName.isEmpty())
        return;

    // Check whether the socket was created
    const QString fileName = m_xdgRuntimeDir + QStringLiteral("/") + m_socketName;
    qCDebug(COMPOSITOR) << "Checking for Wayland socket:" << fileName;
    if (!QFile::exists(fileName))
        return;

    // Do not listen anymore, we found the socket
    m_watcher->removePath(m_xdgRuntimeDir);
    disconnect(m_watcher, SIGNAL(directoryChanged(QString)),
               this, SLOT(socketAvailable()));
    qCDebug(COMPOSITOR) << "Socket" << fileName << "found";
    Q_EMIT started();
}

bool CompositorProcess::respawn()
{
    if (m_retries < 1)
        return false;

    // QProcess will emit finished() after error() but this will make the
    // session quit in nested mode because of the way the full screen shell
    // compositor is connected to the real compositor
    disconnect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
               this, SLOT(processFinished(int,QProcess::ExitStatus)));

    qCDebug(COMPOSITOR)
            << "Process" << m_prog
            << "retries left:" << m_retries;
    m_retries--;

    // Remove watcher
    disconnect(m_watcher, SIGNAL(directoryChanged(QString)),
               this, SLOT(socketAvailable()));
    m_watcher->deleteLater();
    m_watcher = Q_NULLPTR;

    // Destroy the old process
    m_process->deleteLater();
    m_process = Q_NULLPTR;

    // Restart the process after 3 seconds
    QTimer::singleShot(3000, this, SLOT(start()));

    return true;
}

#include "moc_compositorprocess.cpp"
