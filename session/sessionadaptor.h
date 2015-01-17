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

#ifndef SESSIONADAPTOR_H
#define SESSIONADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>

class SessionManager;
class PowerManager;

class SessionAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.hawaii.session")
public:
    SessionAdaptor(SessionManager *sessionManager);

public Q_SLOTS:
    bool canLogOut();
    bool canPowerOff();
    bool canRestart();
    bool canSuspend();
    bool canHibernate();
    bool canHybridSleep();

    Q_NOREPLY void logOut();
    Q_NOREPLY void powerOff();
    Q_NOREPLY void restart();
    Q_NOREPLY void suspend();
    Q_NOREPLY void hibernate();
    Q_NOREPLY void hybridSleep();

private:
    SessionManager *m_sessionManager;
    PowerManager *m_powerManager;
};

#endif // SESSIONADAPTOR_H
