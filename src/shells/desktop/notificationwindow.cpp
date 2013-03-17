/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QGuiApplication>
#include <QQmlEngine>
#include <QQmlComponent>

#include <qpa/qplatformnativeinterface.h>

#include "notificationwindow.h"
#include "waylandintegration.h"

NotificationWindow::NotificationWindow(QObject *parent)
    : QObject(parent)
{
    m_engine = new QQmlEngine(this);
    m_component = new QQmlComponent(m_engine, this);
    m_component->loadUrl(QUrl("qrc:///qml/NotificationItem.qml"));
    if (!m_component->isReady())
        qFatal("Failed to create a notification window: %s",
               qPrintable(m_component->errorString()));

    QObject *topLevel = m_component->create();
    m_window = qobject_cast<QQuickWindow *>(topLevel);
    if (!m_window)
        qFatal("NotificationItem's root item has to be a Window");

    // Create and show window
    m_window->setFlags(Qt::CustomWindow);
    //m_window->create();
}

void NotificationWindow::addSurface()
{
    // Add the surface
    WaylandIntegration *integration = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    struct wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", m_window));
    wl_notification_daemon_add_surface(integration->notification, surface);
}

#include "moc_notificationwindow.cpp"
