// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QtGui/QGuiApplication>

#include <PolkitQt1/Subject>

#include "listener_p.h"
#include "policykitagent.h"

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

Listener::Listener(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
    , m_objectPath(QLatin1String("/io/liri/PolicyKit1/AuthenticationAgent"))
    , m_progressing(false)
    , m_canceled(false)
    , m_session(nullptr)
{
}

bool Listener::registerObject()
{
    qint64 pid = QCoreApplication::instance()->applicationPid();
    PolkitQt1::UnixSessionSubject session(pid);
    return registerListener(session, m_objectPath);
}

void Listener::initiateAuthentication(const QString &actionId,
                                      const QString &message,
                                      const QString &iconName,
                                      const PolkitQt1::Details &details,
                                      const QString &cookie,
                                      const PolkitQt1::Identity::List &identities,
                                      PolkitQt1::Agent::AsyncResult *result)
{
    Q_UNUSED(details);

    // Can't continue if another authorization is in progress
    if (m_progressing) {
        qCWarning(POLKITAGENT, "Another authorization is in progress...");
        return;
    }

    // Reset variables to keep track of this authorization session
    m_progressing = true;
    m_canceled = false;
    m_session = nullptr;

    // Cache cookie: we might need to reused it (to athenticated again
    // upon failure)
    m_cookie = cookie;

    // We don't support more than one identity
    if (identities.size() > 1)
        qCWarning(POLKITAGENT)
                << "Received" << identities.size()
                << "identities, only considering one";

    // uid to authenticate
    uid_t uidToAuthenticate = ::geteuid();
    struct passwd *pw = nullptr;

    // Find the right username
    bool found = false;
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);

        if (identity.toUnixUserIdentity().uid() == ::geteuid()) {
            found = true;
            pw = ::getpwuid(::geteuid());
            m_realName = QString::fromUtf8(pw->pw_gecos);
            uidToAuthenticate = identity.toUnixUserIdentity().uid();
            break;
        }
    }

    if (!found) {
        // Can we find root?
        pw = ::getpwnam("root");
        for (int i = 0; i < identities.size(); i++) {
            PolkitQt1::Identity identity = identities.at(i);

            if (identity.toUnixUserIdentity().uid() == pw->pw_uid) {
                found = true;
                m_realName = tr("Administrator");
                uidToAuthenticate = identity.toUnixUserIdentity().uid();
                break;
            }
        }
    }

    if (!found) {
        PolkitQt1::Identity identity = identities.at(0);
        uidToAuthenticate = identity.toUnixUserIdentity().uid();
        pw = ::getpwuid(uidToAuthenticate);
        m_realName = QString::fromUtf8(pw->pw_gecos);
    }

    // Identity to authenticate
    m_identity = PolkitQt1::UnixUserIdentity(uidToAuthenticate);

    // Associate sessions with identities and initiate session
    PolkitQt1::Agent::Session *session =
            new PolkitQt1::Agent::Session(m_identity, m_cookie, result);
    connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
    connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
    connect(session, SIGNAL(showInfo(QString)), this, SIGNAL(information(QString)));
    connect(session, SIGNAL(showError(QString)), this, SIGNAL(error(QString)));
    session->initiate();

    // Initiate authentication sequence
    // TODO: Find user icon
    Q_EMIT authenticationInitiated(actionId, message, iconName, m_realName);
}

bool Listener::initiateAuthenticationFinish()
{
    // Inform that the authentication sequence has finished
    if (!m_progressing)
        Q_EMIT authenticationFinished();

    return true;
}

void Listener::cancelAuthentication()
{
    qCDebug(POLKITAGENT, "Canceling authentication...");

    // Close current session
    m_progressing = false;
    m_canceled = true;
    m_session = nullptr;

    // Inform that the authentication request was canceled
    Q_EMIT authenticationCanceled();
}

void Listener::request(const QString &request, bool echo)
{
    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qCWarning(POLKITAGENT, "The request() signal was emitted by an invalid session object!");
        return;
    }

    // Cache the session object because it will be used later when the
    // dialog will be either accepted or rejected
    m_session = session;

    // Normalize prompt
    QString prompt = request;
    if (request == QLatin1String("Password:") || request == QLatin1String("Password: "))
        prompt = tr("Password:");

    // Set the prompt and password echo
    Q_EMIT authenticationRequested(prompt, echo);
}

void Listener::completed(bool gainedAuthorization)
{
    qCDebug(POLKITAGENT)
            << "Authorization complete - result:" << gainedAuthorization
            << "canceled:" << m_canceled;

    // Cache async result, will be reused in case of retry
    PolkitQt1::Agent::AsyncResult *result = m_session->result();

    if (gainedAuthorization) {
        // Authorization completed
        Q_EMIT authorizationGained();
    } else {
        // Authorization failed
        qCDebug(POLKITAGENT, "Authorization failed!");

        if (m_canceled) {
            // Inform the dialog the authentication was canceled by the user
            result->setCompleted();
            Q_EMIT authorizationCanceled();
        } else {
            // The user didn't cancel the dialog, this is an actual
            // authentication failure
            Q_EMIT authorizationFailed();

            // Cancel current session
            disconnect(m_session, &PolkitQt1::Agent::Session::request,
                       this, &Listener::request);
            disconnect(m_session, &PolkitQt1::Agent::Session::completed,
                       this, &Listener::completed);
            disconnect(m_session, &PolkitQt1::Agent::Session::showInfo,
                       this, &Listener::information);
            disconnect(m_session, &PolkitQt1::Agent::Session::showError,
                       this, &Listener::error);
            m_session->cancel();
        }

        m_session->deleteLater();
        m_session = nullptr;
    }

    // Complete session only if authorization went fine, otherwise
    // we keep the dialog open and wait for the user to type a good password
    if (gainedAuthorization && !m_canceled) {
        // Unset variables that keep track of the authorization session
        m_progressing = false;

        // Complete session
        m_session->result()->setCompleted();
        m_session->deleteLater();
        m_session = nullptr;
    }

    // Try again if there was an authentication error
    if (!gainedAuthorization && !m_canceled) {
        m_session = new PolkitQt1::Agent::Session(m_identity, m_cookie, result);
        connect(m_session, SIGNAL(request(QString, bool)),
                this, SLOT(request(QString, bool)));
        connect(m_session, SIGNAL(completed(bool)),
                this, SLOT(completed(bool)));
        connect(m_session, SIGNAL(showInfo(QString)),
                this, SLOT(showInfo(QString)));
        connect(m_session, SIGNAL(showError(QString)),
                this, SLOT(showError(QString)));
        m_session->initiate();
    }
}

void Listener::authenticate(const QString &response)
{
    Q_ASSERT(m_session);
    m_canceled = false;
    m_session->setResponse(response);
}

void Listener::abortAuthentication()
{
    qCDebug(POLKITAGENT, "Aborting authentication...");

    // Delete session and inform the dialog that the
    // authentication was canceled so it can be hidden
    m_progressing = false;
    m_canceled = true;
    if (m_session)
        m_session->cancel();
}
