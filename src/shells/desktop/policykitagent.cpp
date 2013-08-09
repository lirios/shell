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
#include "desktopshell.h"
#include "shellui.h"

#include <polkitqt1-subject.h>

Q_GLOBAL_STATIC(PolicyKitAgent, polkitAgent)

PolicyKitAgent::PolicyKitAgent(QObject *parent)
    : PolkitQt1::Agent::Listener(parent)
    , m_progressing(false)
    , m_session(0)
    , m_dialog(0)
{
    PolkitQt1::UnixSessionSubject session(
                QCoreApplication::instance()->applicationPid());
    registerListener(session, "/org/hawaii/PolicyKit1/AuthenticationAgent");
}

PolicyKitAgent::~PolicyKitAgent()
{
    delete m_dialog;
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
    // Can't continue if another authorization is in progress
    if (m_progressing) {
        qDebug() << "Another authorization is in progress...";
        return;
    }

    // Reset variables to keep track of this authorization session
    m_progressing = true;
    m_session = 0;

    // Create the dialog again
    if (m_dialog) {
        QMetaObject::invokeMethod(m_dialog, "hide", Qt::QueuedConnection);
        m_dialog->deleteLater();
    }
    m_dialog = createDialog(actionId, message, iconName, details, identities);
    if (!m_dialog) {
        cancelAuthentication();
        return;
    }

    // Associate sessions with identities and initiate session
    for (int i = 0; i < identities.size(); i++) {
        PolkitQt1::Identity identity = identities.at(i);
        PolkitQt1::Agent::Session *session =
                new PolkitQt1::Agent::Session(identity, cookie, result);
        Q_ASSERT(session);
        m_sessionIdentity[session] = identity;
        connect(session, SIGNAL(request(QString, bool)), this, SLOT(request(QString, bool)));
        connect(session, SIGNAL(completed(bool)), this, SLOT(completed(bool)));
        connect(session, SIGNAL(showInfo(QString)), this, SLOT(showInfo(QString)));
        connect(session, SIGNAL(showError(QString)), this, SLOT(showError(QString)));
        session->initiate();
    }
}

bool PolicyKitAgent::initiateAuthenticationFinish()
{
    if (m_dialog) {
        // Hide the dialog and delete it
        QMetaObject::invokeMethod(m_dialog, "hide", Qt::QueuedConnection);
        m_dialog->deleteLater();
        m_dialog = 0;
    }

    // Unset variables that keep track of the authorization session
    m_progressing = false;
    m_session = 0;

    return true;
}

void PolicyKitAgent::cancelAuthentication()
{
    qDebug() << "Canceling authorization...";

    if (m_dialog) {
        // Hide modal dialog and delete it
        QMetaObject::invokeMethod(m_dialog, "hide", Qt::QueuedConnection);
        m_dialog->deleteLater();
        m_dialog = 0;
    }

    // Unset variables that keep track of the authorization session
    m_progressing = false;
    m_session = 0;
}

void PolicyKitAgent::request(const QString &request, bool echo)
{
    // We need a dialog to continue, fail silently if it doesn't exist
    if (!m_dialog)
        return;

    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Assign the default identity
    PolkitQt1::Identity identity = m_sessionIdentity[session];
    if (identity.isValid()) {
        QtAddOn::AccountsService::AccountsManager manager;
        QtAddOn::AccountsService::UserAccount *account =
                manager.findUserById(identity.toUnixUserIdentity().uid());
        m_dialog->setProperty("defaultIdentity", QVariant::fromValue(account));
        account->deleteLater();
    }

    // Cache the session object because it will be used later when the
    // dialog will be either accepted or rejected
    m_session = session;

    // Set the prompt and password echo
    m_dialog->setProperty("prompt", request);
    m_dialog->setProperty("echo", echo);

    // Show modal dialog
    QMetaObject::invokeMethod(m_dialog, "show", Qt::QueuedConnection);
}

void PolicyKitAgent::completed(bool gainedAuthorization)
{
    // We also need a session, which is the object that emits the signal
    PolkitQt1::Agent::Session *session = qobject_cast<PolkitQt1::Agent::Session *>(sender());
    if (!session) {
        qWarning("The request() signal was emitted by an invalid session object!");
        return;
    }

    // Authorization failed
    if (!gainedAuthorization)
        qDebug() << "Authorization failed!";

    // Authorization session complete
    session->result()->setCompleted();
    m_sessionIdentity.remove(session);
    session->deleteLater();

    // Close modal dialog and delete it
    QMetaObject::invokeMethod(m_dialog, "hide", Qt::QueuedConnection);
    m_dialog->deleteLater();
    m_dialog = 0;

    // Unset variables that keep track of the authorization session
    m_progressing = false;
    m_session = 0;
}

void PolicyKitAgent::showInfo(const QString &text)
{
    qDebug() << "PolicyKit-INFO:" << text;
}

void PolicyKitAgent::showError(const QString &text)
{
    qDebug() << "PolicyKit-ERROR:" << text;
}

QQuickWindow *PolicyKitAgent::createDialog(const QString &actionId,
                                           const QString &message,
                                           const QString &iconName,
                                           const PolkitQt1::Details &details,
                                           const PolkitQt1::Identity::List &identities)
{
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
            return 0;
        }

        // Create the top level object
        QObject *topLevel = component.create();
        if (!topLevel) {
            qWarning() << "Couldn't create an AuthenticationDialog object";
            return 0;
        }

        // Cast it to a window
        QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
        if (!window) {
            qWarning() << "Couldn't cast AuthenticationDialog object to QQuickWindow";
            return 0;
        }

        // Convert the identities list to a list of users
        QtAddOn::AccountsService::AccountsManager manager;
        QList<QVariant> users;
        for (int i = 0; i < identities.size(); i++) {
            PolkitQt1::Identity identity = identities.at(i);
            users << QVariant::fromValue(manager.findUserById(identity.toUnixUserIdentity().uid()));
        }

        // Set all the properties
        topLevel->setProperty("actionId", actionId);
        topLevel->setProperty("message", message);
        topLevel->setProperty("iconName", iconName);
        //topLevel->setProperty("details", details);
        topLevel->setProperty("identities", users);

        // Connect dialog signals
        connect(topLevel, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
        connect(topLevel, SIGNAL(rejected()), this, SLOT(dialogRejected()));

        return window;
    }

    return 0;
}

void PolicyKitAgent::dialogAccepted()
{
    Q_ASSERT(m_dialog);
    Q_ASSERT(m_session);
    m_session->setResponse(m_dialog->property("response").toString());
}

void PolicyKitAgent::dialogRejected()
{
    Q_ASSERT(m_dialog);
    Q_ASSERT(m_session);
    m_session->cancel();
}

#include "moc_policykitagent.cpp"
