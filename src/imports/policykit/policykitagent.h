/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QtQml/QQmlParserStatus>

#include <PolkitQt1/Agent/Listener>

class PolicyKitAgentPrivate;

class PolicyKitAgent : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString objectPath READ objectPath WRITE setObjectPath NOTIFY objectPathChanged)
    Q_PROPERTY(bool registered READ isRegistered NOTIFY registeredChanged)
    Q_DECLARE_PRIVATE(PolicyKitAgent)
    Q_INTERFACES(QQmlParserStatus)
public:
    explicit PolicyKitAgent(QObject *parent = nullptr);
    ~PolicyKitAgent();

    QString objectPath() const;
    void setObjectPath(const QString &path);

    bool isRegistered() const;

    Q_INVOKABLE void registerAgent();
    Q_INVOKABLE void authenticate(const QString &response);
    Q_INVOKABLE void abortAuthentication();

    void classBegin() override {}
    void componentComplete() override;

Q_SIGNALS:
    void objectPathChanged();
    void registeredChanged();

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

private:
    PolicyKitAgentPrivate *const d_ptr;
};
