/*
 * QML Desktop - Set of tools written in C++ for QML
 *
 * Copyright (C) 2017 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2017 Michael Spencer <sonrisesoftware@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>
#include <QtCore/QStandardPaths>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusInterface>
#include <QtWaylandCompositor/QWaylandClient>

#include "applicationmanager.h"

// Applications have 5 seconds to start up before the start animation ends
#define MAX_APPLICATION_STARTUP_TIME (5 * 1000)

Application::Application(const QString &appId, const QStringList &categories, QObject *parent)
    : QObject(parent)
    , m_appId(appId)
    , m_categories(categories)
{
    m_desktopFile = new DesktopFile(appId, this);

    connect(m_desktopFile, &DesktopFile::dataChanged, this, &Application::dataChanged);
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

bool Application::launch(const QStringList &urls)
{
    Q_UNUSED(urls);

    if (!m_desktopFile->isValid())
        return false;

    if (isRunning())
        return true;

    // TODO: Send urls to process launcher
    const QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface interface(QStringLiteral("io.liri.Session"),
                             QStringLiteral("/ProcessLauncher"),
                             QStringLiteral("io.liri.ProcessLauncher"), bus);
    QDBusMessage msg = interface.call(QStringLiteral("launchDesktopFile"), m_desktopFile->path());
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

    for (QWaylandClient *client : qAsConst(m_clients)) {
        m_pids.remove(client->processId());
        client->close();
    }
    m_clients.clear();

    return true;
}

void Application::addClient(QWaylandClient *client)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [client](const QWaylandClient *item) {
        return item->processId() == client->processId();
    });

    if (it == m_clients.end())
        m_clients.append(client);
}
