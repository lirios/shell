/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

    if (m_succeded)
        m_parent->setLocked(false);

    deleteLater();
}
