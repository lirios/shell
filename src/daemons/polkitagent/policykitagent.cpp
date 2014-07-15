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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>

#include <QtAccountsService/AccountsManager>
#include <QtAccountsService/UserAccount>

#include "config.h"
#include "policykitagent.h"
#include "policykitagent_p.h"

#include <PolkitQt1/Subject>

#include <unistd.h>

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

Q_GLOBAL_STATIC(PolicyKitAgent, s_agent)

QT_USE_NAMESPACE_ACCOUNTSSERVICE

using namespace Hawaii;

/*
 * PolicyKitAgentPrivate
 */

PolicyKitAgentPrivate::PolicyKitAgentPrivate(PolicyKitAgent *self)
    : mantle(new Hawaii::Mantle(self))
    , dialog(nullptr)
    , progressing(false)
    , canceled(false)
    , session(0)
    , q_ptr(self)
{
    // Prepare the QML loader
    qmlObject = new QmlObject(self);
    qmlObject->setInitializationDelayed(true);
}

void PolicyKitAgentPrivate::createDialog(const QString &actionId,
                                         const QString &message,
                                         const QString &iconName,
                                         const QString &realName,
                                         const QString &avatar)
{
    Q_Q(PolicyKitAgent);

    // Properties
    QVariantHash props;
    props.insert(QStringLiteral("actionId"), actionId);
    props.insert(QStringLiteral("message"), message);
    props.insert(QStringLiteral("iconName"), iconName);
    props.insert(QStringLiteral("realName"), realName);
    props.insert(QStringLiteral("avatar"), avatar);

    // Load the dialog
    QUrl url = QUrl::fromLocalFile(mantle->shellPackage().filePath("authentication"));
    qmlObject->setSource(url);
    if (!qmlObject->engine() || !qmlObject->engine()->rootContext() ||
            !qmlObject->engine()->rootContext()->isValid() ||
            qmlObject->mainComponent()->isError()) {
        QString errorMsg;
        for (QQmlError error: qmlObject->mainComponent()->errors())
            errorMsg += error.toString() + QStringLiteral("\n");
        errorMsg = q->tr("Unable to load the authentication dialog: %1").arg(errorMsg);
        qFatal(qPrintable(errorMsg));
        return;
    }
    qmlObject->completeInitialization(props);
    dialog = qmlObject->rootObject();

    // Connect signals
    QObject::connect(qmlObject->rootObject(), SIGNAL(authenticationCanceled()),
                     q, SLOT(abortAuthentication()));
    QObject::connect(qmlObject->rootObject(), SIGNAL(authenticationReady(QString)),
                     q, SLOT(authenticate(QString)));
}

void PolicyKitAgentPrivate::destroyDialog()
{
    if (!dialog)
        return;

    dialog->setProperty("visible", false);
    dialog->deleteLater();
    dialog = nullptr;
}

/*
 * PolicyKitAgent
 */

PolicyKitAgent::PolicyKitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
    , d_ptr(new PolicyKitAgentPrivate(this))
{
    PolkitQt1::UnixSessionSubject session(
                QCoreApplication::instance()->applicationPid());
    if (registerListener(session, "/org/hawaii/PolicyKit1/AuthenticationAgent")) {
        qDebug() << "PolicyKit agent started!";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "READY=1\n"
                   "STATUS=PolicyKit agent started\n"
                   "MAINPID=%llu",
                   QCoreApplication::applicationPid());
#endif
    } else {
        const char *msg = "Failed to register PolicyKit agent";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "STATUS=%s\n"
                   "ERRNO=%i",
                   msg, EFAULT);
#endif
        qWarning() << msg;
    }
}

PolicyKitAgent::~PolicyKitAgent()
{
    delete d_ptr;
}

PolicyKitAgent *PolicyKitAgent::instance()
{
    return s_agent();
}

void PolicyKitAgent::initiateAuthentication(const QString &actionId,
                                            const QString &message,
                                            const QString &iconName,
                                            const PolkitQt1::Details &details,
                                            const QString &cookie,
                                            const PolkitQt1::Identity::List &identities,
                                            PolkitQt1::Agent::AsyncResult *result)
{
    Q_D(PolicyKitAgent);

    // Can't continue if another authorization is in progress
    if (d->progressing) {
        qDebug() << "Another authorization is in progress...";
        return;
    }

    // Reset variables to keep track of this authorization session
    d->progressing = true;
    d->canceled = false;
    d->session = 0;

    // Cache cookie: we might need to reused it (to athenticated again
    // upon failure)
    d->cookie = cookie;

    // We don't support more than one identity
    if (identities.size() > 1)
        qDebug() << "PolicyKitAgent: received" << identities.size()
                 << "identities, only considering one";

    // Currently logged in user
    AccountsManager manager;
    UserAccount *account = manager.findUserById(::geteuid());

    // Find the right username
    bool found = false;
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);

        if (identity.toUnixUserIdentity().uid() == account->userId()) {
            found = true;
            break;
        }
    }
    if (!found)
        account = manager.findUserByName(QStringLiteral("root"));

    // Can we find root?
    found = false;
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);

        if (identity.toUnixUserIdentity().uid() == account->userId()) {
            found = true;
            break;
        }
    }
    if (!found) {
        PolkitQt1::Identity identity = identities.at(0);
        uid_t uid = identity.toUnixUserIdentity().uid();
        account = manager.findUserById(uid);
    }

    // Real name
    d->realName = account->realName();

    // Special case for root
    if (account->userName() == QStringLiteral("root"))
        d->realName = tr("Administrator");

    // Identity to authenticate
    d->identity = PolkitQt1::UnixUserIdentity(account->userId());

    // Associate sessions with identities and initiate session
    PolkitQt1::Agent::Session *session =
            new PolkitQt1::Agent::Session(d->identity, d->cookie, result);
    connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
    connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
    connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));
    connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
    session->initiate();

    // Initiate authentication sequence
    d->createDialog(actionId, message, iconName, d->realName,
                    account->iconFileName());
}

bool PolicyKitAgent::initiateAuthenticationFinish()
{
    Q_D(PolicyKitAgent);

    if (!d->progressing) {
        // Inform the dialog that the authentication sequence has finished
        Q_ASSERT(d->dialog);
        QMetaObject::invokeMethod(d->dialog, "authenticationFinished",
                                  Qt::QueuedConnection);
    }

    return true;
}

void PolicyKitAgent::cancelAuthentication()
{
    Q_D(PolicyKitAgent);

    qDebug() << "Canceling authentication...";

    // Close current session
    d->progressing = false;
    d->canceled = true;
    d->session = 0;

    // Hide and destroy the dialog
    d->destroyDialog();
}

void PolicyKitAgent::request(const QString &request, bool echo)
{
    Q_D(PolicyKitAgent);

    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Cache the session object because it will be used later when the
    // dialog will be either accepted or rejected
    d->session = session;

    // Normalize prompt
    QString prompt = request;
    if (request == QStringLiteral("Password:") || request == QStringLiteral("Password: "))
        prompt = tr("Password:");

    // Set the prompt and password echo
    Q_ASSERT(d->dialog);
    d->dialog->setProperty("prompt", prompt);
    d->dialog->setProperty("echo", echo);
    d->dialog->setProperty("visible", true);
}

void PolicyKitAgent::completed(bool gainedAuthorization)
{
    Q_D(PolicyKitAgent);

    qDebug() << "Authorization complete - result:" << gainedAuthorization
             << "canceled:" << d->canceled;

    // Cache async result, will be reused in case of retry
    PolkitQt1::Agent::AsyncResult *result = d->session->result();

    if (gainedAuthorization) {
        // Authorization completed, destroy dialog
        d->destroyDialog();
    } else {
        // Authorization failed
        qDebug() << "Authorization failed!";

        if (d->canceled) {
            // Inform the dialog the authentication was canceled by the user
            result->setCompleted();
            d->destroyDialog();
        } else {
            // The user didn't cancel the dialog, this is an actual
            // authentication failure
            Q_ASSERT(d->dialog);
            d->dialog->setProperty("errorMessage",
                                   tr("Sorry, that didn't work. Please try again."));

            // Cancel current session
            disconnect(d->session, SIGNAL(request(QString, bool)),
                       this, SLOT(request(QString, bool)));
            disconnect(d->session, SIGNAL(completed(bool)),
                       this, SLOT(completed(bool)));
            disconnect(d->session, SIGNAL(showInfo(QString)),
                       this, SLOT(showInfo(QString)));
            disconnect(d->session, SIGNAL(showError(QString)),
                       this, SLOT(showError(QString)));
            d->session->cancel();
        }

        d->session->deleteLater();
        d->session = 0;
    }

    // Complete session only if authorization went fine, otherwise
    // we keep the dialog open and wait for the user to type a good password
    if (gainedAuthorization && !d->canceled) {
        // Unset variables that keep track of the authorization session
        d->progressing = false;

        // Complete session
        d->session->result()->setCompleted();
        d->session->deleteLater();
        d->session = 0;
    }

    // Try again if there was an authentication error
    if (!gainedAuthorization && !d->canceled) {
        d->session = new PolkitQt1::Agent::Session(d->identity, d->cookie, result);
        connect(d->session, SIGNAL(request(QString, bool)),
                this, SLOT(request(QString, bool)));
        connect(d->session, SIGNAL(completed(bool)),
                this, SLOT(completed(bool)));
        connect(d->session, SIGNAL(showInfo(QString)),
                this, SLOT(showInfo(QString)));
        connect(d->session, SIGNAL(showError(QString)),
                this, SLOT(showError(QString)));
        d->session->initiate();
    }
}

void PolicyKitAgent::showInfo(const QString &text)
{
    Q_D(PolicyKitAgent);
    Q_ASSERT(d->dialog);
    d->dialog->setProperty("infoMessage", text);
}

void PolicyKitAgent::showError(const QString &text)
{
    Q_D(PolicyKitAgent);
    Q_ASSERT(d->dialog);
    d->dialog->setProperty("errorMessage", text);
}

void PolicyKitAgent::authenticate(const QString &response)
{
    Q_D(PolicyKitAgent);

    Q_ASSERT(d->session);
    d->canceled = false;
    d->session->setResponse(response);
}

void PolicyKitAgent::abortAuthentication()
{
    Q_D(PolicyKitAgent);

    qDebug() << "Aborting authentication...";

    Q_ASSERT(d->session);

    // Delete session and inform the dialog that the
    // authentication was canceled so it can be hidden
    d->progressing = false;
    d->canceled = true;
    d->session->cancel();
}

#include "moc_policykitagent.cpp"
