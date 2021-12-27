// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>

#include <LiriAuroraCompositor/WaylandClient>

#include "applicationmanager.h"

using namespace Aurora::Compositor;

// Applications have 5 seconds to start up before the start animation ends
#define MAX_APPLICATION_STARTUP_TIME (5 * 1000)

/*
 * DesktopFileAction
 */

DesktopFileAction::DesktopFileAction(QObject *parent)
    : QObject(parent)
{
}

QString DesktopFileAction::name() const
{
    return m_name;
}

QString DesktopFileAction::comment() const
{
    return m_comment;
}

QString DesktopFileAction::command() const
{
    return m_command;
}

/*
 * Application
 */

Application::Application(const QString &appId, const QStringList &categories, QObject *parent)
    : QObject(parent)
    , m_appId(appId)
    , m_categories(categories)
{
    QString fileName = QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                              appId + QStringLiteral(".desktop"));
    m_desktopFile = new Liri::DesktopFile();
    if (m_desktopFile->load(fileName)) {
        const auto actionNames = m_desktopFile->actionNames();
        for (const auto &actionName : actionNames) {
            Liri::DesktopFileAction desktopFileAction = m_desktopFile->action(actionName);
            DesktopFileAction *action = new DesktopFileAction();
            action->m_name = desktopFileAction.name();
            action->m_comment = desktopFileAction.comment();
            action->m_command = desktopFileAction.exec();
            m_actions.append(action);
        }

        emit dataChanged();
    }
}

Application::~Application()
{
    delete m_desktopFile;
    m_desktopFile = nullptr;

    qDeleteAll(m_actions);
}

bool Application::hasCategory(const QString &category) const
{
    return m_categories.contains(category);
}

void Application::setPinned(bool pinned)
{
    if (pinned == m_pinned)
        return;

    m_pinned = pinned;
    emit pinnedChanged();
}

void Application::setState(State state)
{
    if (state == m_state)
        return;

    if (state == State::NotRunning)
        m_active = false;

    m_state = state;
    emit stateChanged();
}

void Application::setActive(bool active)
{
    if (active == m_active)
        return;

    m_active = active;
    emit activeChanged();
}

QQmlListProperty<DesktopFileAction> Application::actions()
{
    auto countFunc = [](QQmlListProperty<DesktopFileAction> *prop) {
        return static_cast<Application *>(prop->object)->m_actions.count();
    };
    auto atFunc = [](QQmlListProperty<DesktopFileAction> *prop, int i) {
        return static_cast<Application *>(prop->object)->m_actions.at(i);
    };
    return QQmlListProperty<DesktopFileAction>(this, nullptr, countFunc, atFunc);
}

bool Application::launch(const QStringList &urls)
{
    Q_UNUSED(urls);

    if (!m_desktopFile->isValid())
        return false;

    if (isRunning())
        return true;

    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface(QStringLiteral("io.liri.Launcher"),
                             QStringLiteral("/io/liri/Launcher"),
                             QStringLiteral("io.liri.Launcher"), bus);
    QDBusMessage msg = interface.call(QStringLiteral("LaunchDesktopFile"), m_desktopFile->fileName());
    bool ran = msg.arguments().at(0).toBool();

    if (ran) {
        setState(Application::Starting);

        QTimer::singleShot(MAX_APPLICATION_STARTUP_TIME, [=]() {
            if (isStarting())
                setState(Application::NotRunning);
        });

        Q_EMIT launched();
    }

    return ran;
}

bool Application::quit()
{
    if (!isRunning())
        return false;

    for (WaylandClient *client : qAsConst(m_clients)) {
        m_pids.remove(client->processId());
        client->close();
    }
    m_clients.clear();

    return true;
}

void Application::addClient(WaylandClient *client)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [client](const WaylandClient *item) {
        return item->processId() == client->processId();
    });

    if (it == m_clients.end())
        m_clients.append(client);
}
