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

#include <qpa/qplatformnativeinterface.h>

#include "notificationwindow.h"
#include "waylandintegration.h"

NotificationWindow::NotificationWindow(QWindow *parent)
    : QQuickWindow(parent)
    , m_surfaceAdded(false)
{
    // Set custom surface format
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setAlphaBufferSize(8);
    setFormat(format);

    // Avoid shell surface
    setFlags(flags() | Qt::BypassWindowManagerHint);

    // Create platform window
    create();
}

void NotificationWindow::addSurface()
{
    // Don't add the surface twice
    if (m_surfaceAdded)
        return;

    // Add the surface
    WaylandIntegration *integration = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    struct wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));
    wl_notification_daemon_add_surface(integration->notification, surface);
    m_surfaceAdded = true;
}

#include "moc_notificationwindow.cpp"
