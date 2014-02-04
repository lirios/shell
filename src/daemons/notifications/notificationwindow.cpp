/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQuick/QQuickItem>

#include "notificationwindow.h"
#include "registrylistener.h"

NotificationWindow::NotificationWindow(QQmlEngine *engine, QWindow *parent)
    : QQuickView(engine, parent)
{
    // Set transparent color
    setColor(Qt::transparent);

    // Avoid shell surface
    setFlags(Qt::BypassWindowManagerHint);

    // Load QML source
    setResizeMode(QQuickView::SizeViewToRootObject);
    setSource(QUrl("qrc:///qml/NotificationBubble.qml"));

    // Create platform window
    create();
}

void NotificationWindow::setInputRegion(const QRect &region)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_compositor *compositor = static_cast<wl_compositor *>(
                native->nativeResourceForIntegration("compositor"));
    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    wl_region *r = wl_compositor_create_region(compositor);
    wl_region_add(r, region.x(), region.y(), region.width(), region.height());
    wl_surface_set_input_region(surface, r);
    wl_region_destroy(r);
}

void NotificationWindow::addSurface()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", window));

    RegistryListener::instance()->addSurface(surface);
}

#include "moc_notificationwindow.cpp"
