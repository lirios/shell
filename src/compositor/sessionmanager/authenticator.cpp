// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "authenticator.h"

#include <security/pam_appl.h>
#include <unistd.h>
#include <pwd.h>

Authenticator::Authenticator(QObject *parent)
    : QObject(parent)
    , m_response(nullptr)
{
}

Authenticator::~Authenticator()
{
}

void Authenticator::authenticate(const QString &password)
{
    const pam_conv conversation = { conversationHandler, this };
    pam_handle_t *handle = nullptr;

    passwd *pwd = getpwuid(getuid());
    if (!pwd) {
        Q_EMIT authenticationError();
        return;
    }

    int retval = pam_start("su", pwd->pw_name, &conversation, &handle);
    if (retval != PAM_SUCCESS) {
        qWarning("pam_start returned %d", retval);
        Q_EMIT authenticationError();
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

        return;
    }

    retval = pam_end(handle, retval);
    if (retval != PAM_SUCCESS) {
        qWarning("pam_end returned %d", retval);
        Q_EMIT authenticationError();
        return;
    }

    Q_EMIT authenticationSucceded();
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

#include "moc_authenticator.cpp"
