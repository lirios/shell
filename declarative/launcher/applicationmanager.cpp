/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QThread>

#include <KWayland/Client/event_queue.h>

#include "applicationmanager.h"
#include "applications.h"
#include "windows.h"

using namespace KWayland::Client;

ApplicationManager::ApplicationManager(QObject *parent)
    : QObject(parent)
    , m_connectionThread(new QThread(this))
    , m_connectionThreadObject(new ConnectionThread())
    , m_eventQueue(Q_NULLPTR)
    , m_apps(new Applications(this))
    , m_windows(new Windows(this))
{
    // Connect to Wayland
    if (!qEnvironmentVariableIsEmpty("GREENISLAND_SOCKET"))
        m_connectionThreadObject->setSocketName(QString::fromUtf8(qgetenv("GREENISLAND_SOCKET")));
    initializeConnection();
}

ApplicationManager::~ApplicationManager()
{
    // Delete members
    delete m_apps;

    // Disconnect from Wayland
    m_connectionThread->quit();
    m_connectionThread->wait();
    m_connectionThreadObject->deleteLater();
}

void ApplicationManager::initializeConnection()
{
    connect(m_connectionThreadObject, &ConnectionThread::connected, this, [this] {
        m_eventQueue = new EventQueue(this);
        m_eventQueue->setup(m_connectionThreadObject);

        Registry *registry = new Registry(this);
        setupRegistry(registry);
    }, Qt::QueuedConnection);
    m_connectionThreadObject->moveToThread(m_connectionThread);
    m_connectionThread->start();

    m_connectionThreadObject->initConnection();
}

void ApplicationManager::setupRegistry(KWayland::Client::Registry *registry)
{
    connect(registry, &Registry::interfaceAnnounced, this,
            [this, registry](QByteArray interface, quint32 name, quint32 version) {
        if (interface == QByteArray("greenisland_applications")) {
            m_apps->init(registry->registry(), name, version);
            m_appsName = name;
        } else if (interface == QByteArray("greenisland_windows")) {
            m_windows->init(registry->registry(), name, version);
            m_windowsName = name;
        }
    });
    connect(registry, &Registry::interfaceRemoved, this,
            [this, registry](quint32 name) {
        if (name == m_appsName) {
            delete m_apps;
            m_apps = Q_NULLPTR;
        } else if (name == m_windowsName) {
            m_windows->deleteLater();
            m_windows = Q_NULLPTR;
        }
    });

    registry->setEventQueue(m_eventQueue);
    registry->create(m_connectionThreadObject);
    registry->setup();
}

void ApplicationManager::quit(const QString &appId)
{
    if (m_apps)
        m_apps->quit(appId);
}

#include "moc_applicationmanager.cpp"
