// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIRI_POLICYKITAGENT_H
#define LIRI_POLICYKITAGENT_H

#include <QtCore/QLoggingCategory>
#include <QtQml/QQmlParserStatus>

#include <PolkitQt1/Agent/Listener>

Q_DECLARE_LOGGING_CATEGORY(POLKITAGENT)

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

#endif // LIRI_POLICYKITAGENT_H
