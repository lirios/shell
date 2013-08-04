/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef POLICYKITAGENT_H
#define POLICYKITAGENT_H

#define POLKIT_AGENT_I_KNOW_API_IS_SUBJECT_TO_CHANGE 1

#include <polkit-qt-1/polkitqt1-agent-session.h>
#include <polkit-qt-1/polkitqt1-identity.h>
#include <polkit-qt-1/polkitqt1-details.h>
#include <polkit-qt-1/polkitqt1-agent-listener.h>

class QQuickWindow;

class PolicyKitAgent : public PolkitQt1::Agent::Listener
{
    Q_OBJECT
public:
    explicit PolicyKitAgent(QObject *parent = 0);
    ~PolicyKitAgent();

    static PolicyKitAgent *instance();

public slots:
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

private:
    bool m_progressing;
    PolkitQt1::Agent::Session *m_session;
    QHash<PolkitQt1::Agent::Session *, PolkitQt1::Identity> m_sessionIdentity;
    QQuickWindow *m_dialog;

    QQuickWindow *createDialog(const QString &actionId,
                             const QString &message,
                             const QString &iconName,
                             const PolkitQt1::Details &details,
                             const PolkitQt1::Identity::List &identities);

private slots:
    void dialogAccepted();
    void dialogRejected();
};

#endif // POLICYKITAGENT_H
