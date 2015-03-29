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
#include <QtCore/QTimer>

#include "compositorlauncher.h"
#include "respawnprocess.h"

#include <signal.h>

RespawnProcess::RespawnProcess(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_retries(5)
    , m_terminate(false)
{
    m_process->setProcessChannelMode(QProcess::ForwardedChannels);

    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(processError(QProcess::ProcessError)));
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(processFinished(int,QProcess::ExitStatus)));
}

void RespawnProcess::start()
{
    qCWarning(COMPOSITOR)
            << "Starting:"
            << qPrintable(m_process->program())
            << qPrintable(m_process->arguments().join(' '));

    m_process->start();
}

void RespawnProcess::stop()
{
    qCWarning(COMPOSITOR)
            << "Stopping:"
            << qPrintable(m_process->program())
            << qPrintable(m_process->arguments().join(' '));

    m_process->terminate();
    if (!m_process->waitForFinished())
        m_process->kill();
    Q_EMIT stopped();
}

void RespawnProcess::terminate()
{
    qCWarning(COMPOSITOR)
            << "Terminate:"
            << qPrintable(m_process->program())
            << qPrintable(m_process->arguments().join(' '));

    m_terminate = true;
    ::kill(m_process->processId(), SIGINT);
}

void RespawnProcess::processError(QProcess::ProcessError error)
{
    Q_UNUSED(error)

    if (m_terminate) {
        m_terminate = false;
        return;
    }

    qCWarning(COMPOSITOR)
            << "Process" << m_process->program()
            << "had an error:" << m_process->errorString();

    // Retry
    if (m_retries > 0) {
        qCDebug(COMPOSITOR)
                << "Process" << m_process->program()
                << "retries left:" << m_retries;
        m_retries--;

        // Restart the process after 3 seconds
        QTimer::singleShot(3000, this, SLOT(start()));
        return;
    }

    // Compositor failed to start, kill full screen shell and terminate
    qCritical(COMPOSITOR)
            << "Process" << m_process->program()
            << "won't start, aborting...";
    QCoreApplication::quit();
}

void RespawnProcess::processFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status)

    if (exitCode == 0) {
        qCWarning(COMPOSITOR)
                << "Process" << m_process->program()
                << "finished successfully";
        Q_EMIT finished();
    } else {
        qCWarning(COMPOSITOR)
                << "Process" << m_process->program()
                << "finished with exit code" << exitCode;
        Q_EMIT stopped();
    }
}

#include "moc_respawnprocess.cpp"
