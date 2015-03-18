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

#ifndef PROCESSLAUNCHER_H
#define PROCESSLAUNCHER_H

#include <QtCore/QLoggingCategory>
#include <QtCore/QMap>
#include <QtDBus/QDBusAbstractAdaptor>

Q_DECLARE_LOGGING_CATEGORY(LAUNCHER)

class QProcess;
class XdgDesktopFile;
class SessionManager;

typedef QMap<QString, QProcess *> ApplicationMap;
typedef QMutableMapIterator<QString, QProcess *> ApplicationMapIterator;

class ProcessLauncher : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.hawaii.launcher")
public:
    ProcessLauncher(SessionManager *sessionManager);
    ~ProcessLauncher();

    bool registerInterface();

    void closeApplications();
    
    static constexpr const char *interfaceName = "org.hawaii.launcher";
    static constexpr const char *objectPath = "/Launcher";

public Q_SLOTS:
    bool launchApplication(const QString &appId);
    bool launchDesktopFile(const QString &fileName);

    bool closeApplication(const QString &appId);
    bool closeDesktopFile(const QString &fileName);

private:
    SessionManager *m_sessionManager;
    ApplicationMap m_apps;

    bool launchEntry(XdgDesktopFile *entry);
    bool closeEntry(const QString &fileName);

    friend class SessionManager;

private Q_SLOTS:
    void finished(int exitCode);
};

#endif // PROCESSLAUNCHER_H
