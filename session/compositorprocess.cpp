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

#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>

#include "compositorlauncher.h"
#include "compositorprocess.h"

CompositorProcess::CompositorProcess(QObject *parent)
    : RespawnProcess(parent)
    , m_xdgRuntimeDir(QString::fromUtf8(qgetenv("XDG_RUNTIME_DIR")))
    , m_watcher(new QFileSystemWatcher(this))
{
    m_watcher->addPath(m_xdgRuntimeDir);
    connect(m_watcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(socketAvailable()));
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
    m_process->setProgram(prog);
}

void CompositorProcess::setArguments(const QStringList &args)
{
    m_process->setArguments(args);
}

void CompositorProcess::setEnvironment(const QProcessEnvironment &env)
{
    m_process->setProcessEnvironment(env);
}

void CompositorProcess::socketAvailable()
{
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

#include "moc_compositorprocess.cpp"
