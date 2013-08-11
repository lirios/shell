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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>
#include <QtQuick/QQuickWindow>

#include <QtAccountsService/AccountsManager>
#include <QtAccountsService/UserAccount>

#include "policykitagent.h"
#include "policykitagent_p.h"
#include "desktopshell.h"
#include "shellui.h"

#include <polkitqt1-subject.h>

#include <unistd.h>

QT_USE_NAMESPACE_ACCOUNTSSERVICE

/*
 * PolicyKitAgentPrivate
 */

PolicyKitAgentPrivate::PolicyKitAgentPrivate(PolicyKitAgent *self)
    : progressing(false)
    , canceled(false)
    , session(0)
    , dialog()
    , q_ptr(self)
{
}

PolicyKitAgentPrivate::~PolicyKitAgentPrivate()
{
    delete dialog;
}

void PolicyKitAgentPrivate::createDialog(const QString &actionId,
                                         const QString &message,
                                         const QString &iconName,
                                         const PolkitQt1::Details &details,
                                         const PolkitQt1::Identity::List &identities)
{
    Q_Q(PolicyKitAgent);

    foreach (ShellUi *ui, DesktopShell::instance()->windows()) {
        // We create the dialog only on the primary screen
        if (ui->screen() != QGuiApplication::primaryScreen())
            continue;

        // Create the component
        QQmlComponent component(ui->engine());
        component.loadUrl(QUrl("qrc:/qml/AuthenticationDialog.qml"));
        if (!component.isReady()) {
            qWarning("Couldn't create AuthenticationDialog component: %s",
                     qPrintable(component.errorString()));
            return;
        }

        // Create the top level object
        QObject *topLevel = component.create();
        if (!topLevel) {
            qWarning() << "Couldn't create an AuthenticationDialog object";
            return;
        }

        // Cast it to a window
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        if (!window) {
            qWarning() << "Couldn't cast AuthenticationDialog object to QQuickWindow";
            return;
        }

        // Set all the properties
        topLevel->setProperty("actionId", actionId);
        topLevel->setProperty("message", message);
        topLevel->setProperty("iconName", iconName);
        //topLevel->setProperty("details", details);
        topLevel->setProperty("errorMessage", QStringLiteral(""));

        // Connect dialog signals
        QObject::connect(topLevel, SIGNAL(authenticate()),
                         q, SLOT(dialogAccepted()));
        QObject::connect(topLevel, SIGNAL(rejected()),
                         q, SLOT(dialogRejected()));

        // Save pointer to the dialog
        dialog = window;
    }
}

void PolicyKitAgentPrivate::dialogAccepted()
{
    Q_ASSERT(dialog);
    Q_ASSERT(session);

    canceled = false;
    session->setResponse(dialog->property("response").toString());
}

void PolicyKitAgentPrivate::dialogRejected()
{
    Q_ASSERT(dialog);
    Q_ASSERT(session);

    canceled = true;
    session->cancel();
}

/*
 * PolicyKitAgent
 */

Q_GLOBAL_STATIC(PolicyKitAgent, polkitAgent)

PolicyKitAgent::PolicyKitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
{
    d_ptr = new PolicyKitAgentPrivate(this);

    PolkitQt1::UnixSessionSubject session(
                QCoreApplication::instance()->applicationPid());
    registerListener(session, "/org/hawaii/PolicyKit1/AuthenticationAgent");
}

PolicyKitAgent::~PolicyKitAgent()
{
    delete d_ptr;
}

PolicyKitAgent *PolicyKitAgent::instance()
{
    return polkitAgent();
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

    // Create the dialog again
    if (d->dialog) {
        QMetaObject::invokeMethod(d->dialog, "hide", Qt::QueuedConnection);
        d->dialog->deleteLater();
    }
    d->createDialog(actionId, message, iconName, details, identities);
    if (!d->dialog) {
        cancelAuthentication();
        return;
    }

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

    // Set user identity on the dialog
    d->dialog->setProperty("realName", d->realName);
    d->dialog->setProperty("avatar", account->iconFileName());

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
}

bool PolicyKitAgent::initiateAuthenticationFinish()
{
    Q_D(PolicyKitAgent);

    if (d->dialog && !d->progressing) {
        // Hide the dialog and delete it
        QMetaObject::invokeMethod(d->dialog, "hide", Qt::QueuedConnection);
        d->dialog->deleteLater();
        d->dialog = 0;
    }

    return true;
}

void PolicyKitAgent::cancelAuthentication()
{
    Q_D(PolicyKitAgent);

    qDebug() << "Canceling authorization...";

    if (d->dialog) {
        // Hide modal dialog and delete it
        QMetaObject::invokeMethod(d->dialog, "hide", Qt::QueuedConnection);
        d->dialog->deleteLater();
        d->dialog = 0;
    }

    // Unset variables that keep track of the authorization session
    d->progressing = false;
    d->canceled = false;
    d->session = 0;
}

void PolicyKitAgent::request(const QString &request, bool echo)
{
    Q_D(PolicyKitAgent);

    // We need a dialog to continue, fail silently if it doesn't exist
    if (!d->dialog)
        return;

    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Cache the session object because it will be used later when the
    // dialog will be either accepted or rejected
    d->session = session;

    // Set the prompt and password echo
    d->dialog->setProperty("prompt", request);
    d->dialog->setProperty("echo", echo);

    // Show modal dialog
    QMetaObject::invokeMethod(d->dialog, "show", Qt::QueuedConnection);
}

void PolicyKitAgent::completed(bool gainedAuthorization)
{
    Q_D(PolicyKitAgent);

    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Cache async result, will be reused in case of retry
    PolkitQt1::Agent::AsyncResult *result = d->session->result();

    if (gainedAuthorization) {
        // Authorization ok
        if (d->dialog) {
            // Hide modal dialog and delete it
            QMetaObject::invokeMethod(d->dialog, "hide", Qt::QueuedConnection);
            d->dialog->deleteLater();
            d->dialog = 0;
        }
    } else {
        // Authorization failed
        qDebug() << "Authorization failed!";

        if (!d->canceled) {
            // The user didn't cancel the dialog, this is an actual
            // authentication failure
            d->dialog->setProperty("errorMessage",
                                   tr("Sorry, that didn't work. Please try again."));

            // Cancel current session
            disconnect(session, SIGNAL(request(QString, bool)),
                       this, SLOT(request(QString, bool)));
            disconnect(session, SIGNAL(completed(bool)),
                       this, SLOT(completed(bool)));
            disconnect(session, SIGNAL(showInfo(QString)),
                       this, SLOT(showInfo(QString)));
            disconnect(session, SIGNAL(showError(QString)),
                       this, SLOT(showError(QString)));
            session->cancel();
            session->deleteLater();
            d->session = 0;
        }
    }

    // Complete session only if authorization went fine, otherwise
    // we keep the dialog open and wait for the user to type a good password
    if (gainedAuthorization || d->canceled) {
        // Unset variables that keep track of the authorization session
        d->progressing = false;
        d->session = 0;

        // Complete session
        session->result()->setCompleted();
        session->deleteLater();
    }

    // Try again if there was an authentication error
    if (!gainedAuthorization && !d->canceled) {
        session = new PolkitQt1::Agent::Session(d->identity, d->cookie, result);
        connect(session, SIGNAL(request(QString, bool)),
                this, SLOT(request(QString, bool)));
        connect(session, SIGNAL(completed(bool)),
                this, SLOT(completed(bool)));
        connect(session, SIGNAL(showInfo(QString)),
                this, SLOT(showInfo(QString)));
        connect(session, SIGNAL(showError(QString)),
                this, SLOT(showError(QString)));
        d->session = session;
        session->initiate();
    }
}

void PolicyKitAgent::showInfo(const QString &text)
{
    qDebug() << "PolicyKit-INFO:" << text;
}

void PolicyKitAgent::showError(const QString &text)
{
    qDebug() << "PolicyKit-ERROR:" << text;
}

#include "moc_policykitagent.cpp"
