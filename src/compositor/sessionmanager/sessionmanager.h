// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
