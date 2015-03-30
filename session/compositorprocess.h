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

#include "respawnprocess.h"

class QFileSystemWatcher;

class CompositorProcess : public RespawnProcess
{
    Q_OBJECT
public:
    CompositorProcess(bool sessionLeader, QObject *parent = 0);

    QString socketName() const;
    void setSocketName(const QString &name);

    void setProgram(const QString &prog);
    void setArguments(const QStringList &args);
    void setEnvironment(const QProcessEnvironment &env);

private:
    QString m_xdgRuntimeDir;
    QString m_socketName;
    QFileSystemWatcher *m_watcher;

private Q_SLOTS:
    void socketAvailable();
};

#endif // COMPOSITORPROCESS_H
