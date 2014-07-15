/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POLICYKITAGENT_H
#define POLICYKITAGENT_H

#include <PolkitQt1/Agent/Listener>

class PolicyKitAgentPrivate;

class PolicyKitAgent : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
public:
    explicit PolicyKitAgent(QObject *parent = 0);
    ~PolicyKitAgent();

    static PolicyKitAgent *instance();

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
    void showInfo(const QString &text);
    void showError(const QString &text);

    void authenticate(const QString &response);
    void abortAuthentication();

private:
    Q_DISABLE_COPY(PolicyKitAgent)
    Q_DECLARE_PRIVATE(PolicyKitAgent)
    PolicyKitAgentPrivate *d_ptr;
};

#endif // POLICYKITAGENT_H
