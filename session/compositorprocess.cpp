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
    } else if (status == QProcess::NormalExit) {
        qCWarning(COMPOSITOR)
                << "Process" << m_prog
                << "finished with exit code" << exitCode;
        Q_EMIT stopped();
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
