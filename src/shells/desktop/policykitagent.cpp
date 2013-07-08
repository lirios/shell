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

#include <QCoreApplication>
#include <QDebug>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickItem>

#include <QtAccountsService/AccountsManager>
#include <QtAccountsService/UserAccount>

#include "policykitagent.h"
#include "shellview.h"

#include <polkit-qt-1/polkitqt1-subject.h>

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
    if (m_dialog)
        delete m_dialog;
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
    // Delete dialog
    delete m_dialog;
    m_dialog = 0;

    // Unset variables that keep track of the authorization session
    m_progressing = false;
    m_session = 0;

    return true;
}

void PolicyKitAgent::cancelAuthentication()
{
    // Close modal dialog and delete it
    QMetaObject::invokeMethod(m_dialog, "close");
    delete m_dialog;
    m_dialog = 0;

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

    // Open modal dialog
    QMetaObject::invokeMethod(m_dialog, "open");
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
    delete session;

    // Close modal dialog and delete it
    QMetaObject::invokeMethod(m_dialog, "close");
    delete m_dialog;
    m_dialog = 0;

    // Unset variables that keep track of the authorization session
    m_progressing = false;
    m_session = 0;
}

void PolicyKitAgent::showInfo(const QString &text)
{
    qDebug() << "INFO:" << text;
}

void PolicyKitAgent::showError(const QString &text)
{
    qDebug() << "ERROR:" << text;
}

QQuickItem *PolicyKitAgent::createDialog(const QString &actionId,
                                         const QString &message,
                                         const QString &iconName,
                                         const PolkitQt1::Details &details,
                                         const PolkitQt1::Identity::List &identities)
{
    ShellView *view = DesktopShell::instance()->shellView();

    // Create the component
    QQmlComponent component(view->engine(), QUrl("qrc:///qml/AuthenticationDialog.qml"),
                            view->rootObject());
    if (component.isError()) {
        qWarning("Couldn't create AuthenticationDialog component: %s",
                 component.errorString().toLocal8Bit().constData());
        return 0;
    }

    // Create the object
    QObject *object = component.create(view->rootContext());
    if (!object) {
        qWarning() << "Couldn't create an AuthenticationDialog object";
        return 0;
    }

    // Cast it to a QQuickItem
    QQuickItem *item = qobject_cast<QQuickItem *>(object);
    if (!item) {
        qWarning() << "Couldn't cast AuthenticationDialog object to QQuickItem";
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
    item->setParentItem(view->rootObject());
    item->setProperty("actionId", actionId);
    item->setProperty("message", message);
    item->setProperty("iconName", iconName);
    //item->setProperty("details", details);
    item->setProperty("identities", users);

    // Connect dialog signals
    connect(object, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
    connect(object, SIGNAL(rejected()), this, SLOT(dialogRejected()));

    return item;
}

void PolicyKitAgent::dialogAccepted()
{
    Q_ASSERT(m_dialog);
    m_session->setResponse(m_dialog->property("response").toString());
}

void PolicyKitAgent::dialogRejected()
{
    m_session->setResponse(QLatin1String(""));
}

#include "moc_policykitagent.cpp"
