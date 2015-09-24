/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

#include <GreenIsland/Compositor>

#include "application.h"
#include "session/sessioninterface.h"
#include "sessionmanager.h"

using namespace GreenIsland;

Application::Application()
    : QObject()
    , HomeApplication()
    , m_sessionManager(Q_NULLPTR)
{
}

void Application::compositorLaunched()
{
    // Session manager
    m_sessionManager = new SessionManager(this);
    if (!m_sessionManager->initialize())
        QCoreApplication::exit(1);
    m_sessionManager->autostart();

    // Idle and wake signals
    connect(Compositor::instance(), &Compositor::idle, this, [this] {
        m_sessionManager->setIdle(true);
    });
    connect(Compositor::instance(), &Compositor::wake, this, [this] {
        m_sessionManager->setIdle(false);
    });

    // Register session interface as a fake singleton
    QQmlContext *context = Compositor::instance()->engine()->rootContext();
    context->setContextProperty(QStringLiteral("SessionInterface"),
                                new SessionInterface(m_sessionManager));
}

#include "moc_application.cpp"
