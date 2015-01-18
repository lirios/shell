/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtCore/QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(PROCESS_CONTROLLER)

class QFileSystemWatcher;

class ProcessController : public QObject
{
    Q_OBJECT
public:
    ProcessController(const QString &mode, QObject *parent = Q_NULLPTR);

    QString mode() const;

    QString compositorSocket() const;

    void start();
    void stop();

Q_SIGNALS:
    void started();
    void stopped();

private:
    QProcess *m_compositor;
    QStringList m_compositorArgs;
    QString m_compositorSocket;

    QProcess *m_fullScreenShell;
    QStringList m_fullScreenShellArgs;
    QString m_fullScreenShellSocket;
    QFileSystemWatcher *m_fullScreenShellWatcher;

    QString m_mode;
    bool m_hasLibInputPlugin;

    QString randomString() const;

    void prepare();

    void setupFullScreenShell();
    void setupCompositor();

    void printSummary();

private Q_SLOTS:
    void startCompositor();

    void directoryChanged(const QString &path);

    void compositorFinished(int code, const QProcess::ExitStatus &status);
    void fullScreenShellFinished(int code, const QProcess::ExitStatus &status);
};

#endif // PROCESSCONTROLLER_H
