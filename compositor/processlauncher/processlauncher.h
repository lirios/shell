/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef PROCESSLAUNCHER_H
#define PROCESSLAUNCHER_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QProcess>

Q_DECLARE_LOGGING_CATEGORY(LAUNCHER)

class QProcess;
class XdgDesktopFile;

typedef QMap<QString, QProcess *> ApplicationMap;
typedef QMutableMapIterator<QString, QProcess *> ApplicationMapIterator;

class ProcessLauncher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString waylandSocketName READ waylandSocketName WRITE setWaylandSocketName NOTIFY
                   waylandSocketNameChanged)
public:
    ProcessLauncher(QObject *parent = 0);
    ~ProcessLauncher();

    QString waylandSocketName() const;
    void setWaylandSocketName(const QString &name);

    Q_INVOKABLE bool launchApplication(const QString &appId);
    Q_INVOKABLE bool launchDesktopFile(const QString &fileName);
    Q_INVOKABLE bool launchCommand(const QString &command);
    bool launchEntry(const XdgDesktopFile &entry);

    Q_INVOKABLE bool closeApplication(const QString &appId);
    Q_INVOKABLE bool closeDesktopFile(const QString &fileName);
    void closeApplications();

    static bool registerWithDBus(ProcessLauncher *instance);

Q_SIGNALS:
    void waylandSocketNameChanged();

private:
    QString m_waylandSocketName;
    ApplicationMap m_apps;

    bool closeEntry(const QString &fileName);

private Q_SLOTS:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // PROCESSLAUNCHER_H
