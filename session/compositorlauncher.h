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

#ifndef COMPOSITORLAUNCHER_H
#define COMPOSITORLAUNCHER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

#include "compositorprocess.h"

Q_DECLARE_LOGGING_CATEGORY(COMPOSITOR)

class CompositorLauncher : public QObject
{
    Q_OBJECT
public:
    enum Mode {
        UnknownMode = 0,
        EglFSMode,
        HwComposerMode,
        NestedMode,
        X11Mode,
        WaylandMode
    };

    enum Hardware {
        UnknownHardware = 0,
        DrmHardware,
        BroadcomHardware
    };

    CompositorLauncher(QObject *parent = 0);

    Mode mode() const;
    void setMode(const Mode &mode);

    QString socketName() const;
    QString sessionSocketName() const;

public Q_SLOTS:
    void start();
    void stop();
    void terminate();

Q_SIGNALS:
    void started();
    void stopped();
    void finished();

private:
    Mode m_mode;
    Hardware m_hardware;
    QString m_socketName;
    QString m_sessionSocketName;
    bool m_hasLibInputPlugin;
    CompositorProcess *m_weston;
    CompositorProcess *m_compositor;

    void detectMode();
    void detectHardware();

    QString deviceModel() const;
    bool pathsExist(const QStringList &paths) const;

    void detectLibInput();

    QStringList compositorArgs() const;
    QProcessEnvironment compositorEnv() const;

    void printSummary();

    void setupEnvironment();
};

#endif // COMPOSITORLAUNCHER_H
