// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_POLKIT_LISTENER_P_H
#define LIRI_POLKIT_LISTENER_P_H

#include <PolkitQt1/Agent/Listener>

class Listener : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
    Q_DISABLE_COPY(Listener)
public:
    Listener(QObject *parent = nullptr);

    bool registerObject();

    QString m_objectPath;
    bool m_progressing;
    bool m_canceled;
    QString m_realName;
    PolkitQt1::UnixUserIdentity m_identity;
    QString m_cookie;
    PolkitQt1::Agent::Session *m_session;

Q_SIGNALS:
    void authenticationInitiated(const QString &actionId, const QString &message,
                                 const QString &iconName, const QString &realName);
    void authenticationRequested(const QString &prompt, bool echo);
    void authenticationCanceled();
    void authenticationFinished();

    void authorizationGained();
    void authorizationCanceled();
    void authorizationFailed();

    void information(const QString &message);
    void error(const QString &message);

public Q_SLOTS:
    void initiateAuthentication(const QString &actionId,
                                const QString &message,
                                const QString &iconName,
                                const PolkitQt1::Details &details,
                                const QString &cookie,
                                const PolkitQt1::Identity::List &identities,
                                PolkitQt1::Agent::AsyncResult *result);
    bool initiateAuthenticationFinish();

    void cancelAuthentication();

    void request(const QString &request, bool echo);
    void completed(bool gainedAuthorization);

    void authenticate(const QString &response);
    void abortAuthentication();
};

#endif // LIRI_POLKIT_LISTENER_P_H
