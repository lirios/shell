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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>

#include "vthandler.h"

Q_DECLARE_LOGGING_CATEGORY(SESSION_MANAGER)

class ProcessLauncher;
class ScreenSaver;
class SocketServer;

class SessionManager : public QObject
{
    Q_OBJECT
public:
    static SessionManager *instance();

    bool initialize();

    VtHandler *vtHandler() const;

    bool isIdle() const;
    void setIdle(bool value);

    void idleInhibit();
    void idleUninhibit();

    bool isLocked() const;
    void setLocked(bool value);

    static constexpr const char *interfaceName = "org.hawaii.session";
    static constexpr const char *objectPath = "/HawaiiSession";

Q_SIGNALS:
    void idleChanged(bool value);
    void lockedChanged(bool value);
    void loggedOut();

public Q_SLOTS:
    void logOut();

protected:
    SessionManager(QObject *parent = 0);

private:
    ProcessLauncher *m_launcher;
    ScreenSaver *m_screenSaver;
    SocketServer *m_server;
    QList<qint64> m_processes;
    VtHandler *m_vtHandler;

    bool m_idle;
    bool m_locked;

    void setupEnvironment();
    bool registerDBus();

private Q_SLOTS:
    void autostart();
};

#endif // SESSIONMANAGER_H
