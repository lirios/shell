/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef PROCESSCONTROLLER_H
#define PROCESSCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QProcess>

class QFileSystemWatcher;

class ProcessController : public QObject
{
    Q_OBJECT
public:
    ProcessController(bool nested = false, QObject *parent = Q_NULLPTR);

    void start();

private:
    QProcess *m_compositor;
    QStringList m_compositorArgs;
    QString m_compositorSocket;

    QProcess *m_fullScreenShell;
    QStringList m_fullScreenShellArgs;
    QString m_fullScreenShellSocket;
    QFileSystemWatcher *m_fullScreenShellWatcher;

    QString randomString() const;

private Q_SLOTS:
    void startCompositor();

    void directoryChanged(const QString &path);

    void compositorFinished(int code, const QProcess::ExitStatus &status);
    void fullScreenShellFinished(int code, const QProcess::ExitStatus &status);
};

#endif // PROCESSCONTROLLER_H
