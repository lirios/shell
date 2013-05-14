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

#include "shellwindow.h"
#include "waylandintegration.h"

ShellWindow::ShellWindow(QWindow *parent)
    : QQuickWindow(parent)
{
    // Set custom surface format
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setAlphaBufferSize(8);
    setFormat(format);

    // Set custom window type
    // TODO: Also set Qt::BypassWindowManagerHint and call setSpecial()
    setFlags(Qt::Popup);

    // Create platform window and inform the compositor about us
    create();
}

void ShellWindow::exposeEvent(QExposeEvent *event)
{
    QQuickWindow::exposeEvent(event);

    if (isExposed())
        forcePosition();
}

void ShellWindow::setSpecial()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    hawaii_desktop_shell_set_special(object->shell, output, surface);
}

void ShellWindow::forcePosition()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    QPoint pos = position();
    hawaii_desktop_shell_set_position(object->shell, surface, pos.x(), pos.y());
}

#include "moc_shellwindow.cpp"
