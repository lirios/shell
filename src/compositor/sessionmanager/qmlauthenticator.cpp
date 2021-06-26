// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "authenticator.h"
#include "qmlauthenticator.h"

QmlAuthenticator::QmlAuthenticator(SessionManager *parent, const QJSValue &callback)
    : m_parent(parent)
    , m_callback(callback)
    , m_succeded(false)
{
    connect(m_parent->m_authenticator, &Authenticator::authenticationSucceded, this, [=] {
        m_succeded = true;
        authenticate();
    });
    connect(m_parent->m_authenticator, &Authenticator::authenticationFailed, this, [=] {
        m_succeded = false;
        authenticate();
    });
    connect(m_parent->m_authenticator, &Authenticator::authenticationError, this, [=] {
        m_succeded = false;
        authenticate();
    });
}

void QmlAuthenticator::authenticate()
{
    if (m_callback.isCallable())
        m_callback.call(QJSValueList() << m_succeded);

    m_parent->m_authRequested = false;

    deleteLater();
}
