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
public:
    SessionManager(QObject *parent = nullptr);
    virtual ~SessionManager();

public Q_SLOTS:
    void registerService();
    void authenticate(const QString &password, const QJSValue &callback);

private:
    QThread *m_authenticatorThread = nullptr;
    bool m_authRequested = false;
    Authenticator *m_authenticator = nullptr;

    friend class QmlAuthenticator;
};

#endif // SESSIONMANAGER_H
