// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "authenticator.h"

#include <security/pam_appl.h>
#include <unistd.h>
#include <pwd.h>

Authenticator::Authenticator(QObject *parent)
    : QObject(parent)
{
}

Authenticator::~Authenticator()
{
}

void Authenticator::authenticate(const QString &password, const QJSValue &callback)
{
    if (m_authRequested)
        return;

    m_authRequested = true;
    m_callback = callback;
    actualAuthentication(password);
}

Authenticator *Authenticator::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    Q_UNUSED(jsEngine);

    return new Authenticator();
}

void Authenticator::actualAuthentication(const QString &password)
{
    const pam_conv conversation = { conversationHandler, this };
    pam_handle_t *handle = nullptr;

    passwd *pwd = getpwuid(getuid());
    if (!pwd) {
        Q_EMIT authenticationError();
        sendReply(false);
        return;
    }

    int retval = pam_start("su", pwd->pw_name, &conversation, &handle);
    if (retval != PAM_SUCCESS) {
        qWarning("pam_start returned %d", retval);
        Q_EMIT authenticationError();
        sendReply(false);
        return;
    }

    m_response = new pam_response;
    m_response[0].resp = strdup(qPrintable(password));
    m_response[0].resp_retcode = 0;

    retval = pam_authenticate(handle, 0);
    if (retval != PAM_SUCCESS) {
        if (retval == PAM_AUTH_ERR) {
            Q_EMIT authenticationFailed();
        } else {
            qWarning("pam_authenticate returned %d", retval);
            Q_EMIT authenticationError();
        }
        sendReply(false);

        return;
    }

    retval = pam_end(handle, retval);
    if (retval != PAM_SUCCESS) {
        qWarning("pam_end returned %d", retval);
        Q_EMIT authenticationError();
        sendReply(false);
        return;
    }

    Q_EMIT authenticationSucceded();
    sendReply(true);
}

void Authenticator::sendReply(bool succeeded)
{
    if (m_callback.isCallable())
        m_callback.call(QJSValueList() << succeeded);

    m_authRequested = false;
}

int Authenticator::conversationHandler(int num, const pam_message **message,
                                       pam_response **response, void *data)
{
    Q_UNUSED(num);
    Q_UNUSED(message);

    Authenticator *self = static_cast<Authenticator *>(data);
    *response = self->m_response;
    return PAM_SUCCESS;
}
