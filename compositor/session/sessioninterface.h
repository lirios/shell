/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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

#ifndef SESSIONINTERFACE_H
#define SESSIONINTERFACE_H

#include <QtCore/QThread>
#include <QtQml/QJSValue>

class Authenticator;
class CustomAuthenticator;
class SessionManager;

class SessionInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canLock READ canLock CONSTANT)
    Q_PROPERTY(bool canStartNewSession READ canStartNewSession CONSTANT)
    Q_PROPERTY(bool canLogOut READ canLogOut CONSTANT)
    Q_PROPERTY(bool canPowerOff READ canPowerOff CONSTANT)
    Q_PROPERTY(bool canRestart READ canRestart CONSTANT)
    Q_PROPERTY(bool canSuspend READ canSuspend CONSTANT)
    Q_PROPERTY(bool canHibernate READ canHibernate CONSTANT)
    Q_PROPERTY(bool canHybridSleep READ canHybridSleep CONSTANT)
public:
    SessionInterface(SessionManager *sm, QObject *parent = 0);
    virtual ~SessionInterface();

    inline SessionManager *sessionManager() const {
        return m_sessionManager;
    }

    bool canLock() const;
    bool canStartNewSession() const;
    bool canLogOut() const;
    bool canPowerOff() const;
    bool canRestart() const;
    bool canSuspend() const;
    bool canHibernate() const;
    bool canHybridSleep() const;

Q_SIGNALS:
    void sessionLocked();
    void sessionUnlocked();

    void logOutRequested();
    void powerOffRequested();
    void restartRequested();
    void suspendRequested();
    void hibernateRequested();
    void hybridSleepRequested();
    void shutdownRequestCanceled();

public Q_SLOTS:
    void lockSession();
    void unlockSession(const QString &password, const QJSValue &callback);
    void startNewSession();
    void activateSession(int index);

    void requestLogOut();
    void requestPowerOff();
    void requestRestart();
    void requestSuspend();
    void requestHibernate();
    void requestHybridSleep();
    void cancelShutdownRequest();

    void logOut();
    void powerOff();
    void restart();
    void suspend();
    void hibernate();
    void hybridSleep();

private:
    SessionManager *m_sessionManager;
    Authenticator *m_authenticator;
    QThread *m_authenticatorThread;
    bool m_authRequested;


    bool m_canLock;
    bool m_canStartNewSession;
    bool m_canLogOut;
    bool m_canPowerOff;
    bool m_canRestart;
    bool m_canSuspend;
    bool m_canHibernate;
    bool m_canHybridSleep;

    friend class CustomAuthenticator;
};

#endif // SESSIONINTERFACE_H
