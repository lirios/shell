/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef SESSIONINTERFACE_H
#define SESSIONINTERFACE_H

#include <QtDBus/QDBusConnection>
#include <QtQml/QJSValue>

class Authenticator;
class CustomAuthenticator;
class QDBusInterface;

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
    SessionInterface(QObject *parent = 0);
    ~SessionInterface();

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

public Q_SLOTS:
    void lockSession();
    void unlockSession(const QString &password, const QJSValue &callback);
    void startNewSession();
    void activateSession(int index);

    void logOut();
    void powerOff();
    void restart();
    void suspend();
    void hibernate();
    void hybridSleep();

private:
    Authenticator *m_authenticator;
    QThread *m_authenticatorThread;

    bool m_authRequested;

    QDBusInterface *m_interface;
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
