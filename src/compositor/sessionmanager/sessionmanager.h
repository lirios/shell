/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini
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

#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QLoggingCategory>
#include <QtCore/QThread>
#include <QtQml/QJSValue>

Q_DECLARE_LOGGING_CATEGORY(SESSION_MANAGER)

class Authenticator;
class CustomAuthenticator;
class LoginManager;

class SessionManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool idle READ isIdle WRITE setIdle NOTIFY idleChanged)
public:
    SessionManager(QObject *parent = nullptr);
    virtual ~SessionManager();

    bool isIdle() const;
    void setIdle(bool value);

Q_SIGNALS:
    void idleChanged(bool value);
    void sessionLocked();
    void sessionUnlocked();
    void idleInhibitRequested();
    void idleUninhibitRequested();

public Q_SLOTS:
    void registerService();
    void lock();
    void unlock();
    void launchApplication(const QString &appId);
    void launchDesktopFile(const QString &fileName);
    void launchCommand(const QString &command);
    void setEnvironment(const QString &key, const QString &value);
    void authenticate(const QString &password, const QJSValue &callback);

private:
    QThread *m_authenticatorThread = nullptr;
    bool m_authRequested = false;
    Authenticator *m_authenticator = nullptr;

    bool m_idle = false;

    friend class QmlAuthenticator;
};

#endif // SESSIONMANAGER_H
