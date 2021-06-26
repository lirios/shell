// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "listener_p.h"
#include "policykitagent.h"

Q_LOGGING_CATEGORY(POLKITAGENT, "liri.policykit")

/*
 * PolicyKitAgentPrivate
 */

class PolicyKitAgentPrivate
{
public:
    PolicyKitAgentPrivate();
    ~PolicyKitAgentPrivate();

    bool initialized;
    bool registered;
    Listener *listener;
};

PolicyKitAgentPrivate::PolicyKitAgentPrivate()
    : initialized(false)
    , registered(false)
    , listener(new Listener())
{
}

PolicyKitAgentPrivate::~PolicyKitAgentPrivate()
{
    delete listener;
}

/*
 * PolicyKitAgent
 */

PolicyKitAgent::PolicyKitAgent(QObject *parent)
    : QObject(parent)
    , d_ptr(new PolicyKitAgentPrivate())
{
    // Rely signals
    connect(d_func()->listener, &Listener::authenticationInitiated,
            this, &PolicyKitAgent::authenticationInitiated);
    connect(d_func()->listener, &Listener::authenticationRequested,
            this, &PolicyKitAgent::authenticationRequested);
    connect(d_func()->listener, &Listener::authenticationCanceled,
            this, &PolicyKitAgent::authenticationCanceled);
    connect(d_func()->listener, &Listener::authenticationFinished,
            this, &PolicyKitAgent::authenticationFinished);
    connect(d_func()->listener, &Listener::authorizationGained,
            this, &PolicyKitAgent::authorizationGained);
    connect(d_func()->listener, &Listener::authorizationCanceled,
            this, &PolicyKitAgent::authorizationCanceled);
    connect(d_func()->listener, &Listener::authorizationFailed,
            this, &PolicyKitAgent::authorizationFailed);
    connect(d_func()->listener, &Listener::information,
            this, &PolicyKitAgent::information);
    connect(d_func()->listener, &Listener::error,
            this, &PolicyKitAgent::error);
}

PolicyKitAgent::~PolicyKitAgent()
{
    delete d_ptr;
}

QString PolicyKitAgent::objectPath() const
{
    Q_D(const PolicyKitAgent);
    return d->listener->m_objectPath;
}

void PolicyKitAgent::setObjectPath(const QString &path)
{
    Q_D(PolicyKitAgent);

    if (d->listener->m_objectPath == path)
        return;

    if (d->initialized) {
        qCWarning(POLKITAGENT, "Cannot change PolicyKitAgent::objectPath after initialization");
        return;
    }

    d->listener->m_objectPath = path;
    Q_EMIT objectPathChanged();
}

bool PolicyKitAgent::isRegistered() const
{
    Q_D(const PolicyKitAgent);
    return d->registered;
}

void PolicyKitAgent::registerAgent()
{
    Q_D(PolicyKitAgent);

    if (d->registered) {
        qCWarning(POLKITAGENT, "Cannot register PolicyKitAgent more than once");
        return;
    }

    d->registered = d->listener->registerObject();
    if (d->registered)
        Q_EMIT registeredChanged();
}

void PolicyKitAgent::authenticate(const QString &response)
{
    Q_D(PolicyKitAgent);
    d->listener->authenticate(response);
}

void PolicyKitAgent::abortAuthentication()
{
    Q_D(PolicyKitAgent);
    d->listener->abortAuthentication();
}

void PolicyKitAgent::componentComplete()
{
    Q_D(PolicyKitAgent);
    d->initialized = true;
}
