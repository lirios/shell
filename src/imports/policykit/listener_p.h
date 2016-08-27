/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#pragma once

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
