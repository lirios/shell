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

#include <QtCompositor/QWaylandSurfaceItem>

#include <private/qwlsurface_p.h>

#include "shell.h"
#include "compositor.h"

Shell::Shell(struct ::wl_display *display)
    : QObject()
    , QtWaylandServer::wl_hawaii_shell(display)
{
}

QWaylandSurface *Shell::surfaceAt(const QPointF &point, QPointF *local)
{
    QWaylandSurface *surface;

#define SURFACE_AT(layer) \
    surface = surfaceAt(layer, point, local); \
    if (surface) \
        return surface;

    SURFACE_AT(m_panelsLayer);
    SURFACE_AT(m_dialogsLayer);
    SURFACE_AT(m_overlayLayer);
    SURFACE_AT(m_backgroundLayer);

#undef SURFACE_AT

    return 0;
}

QWaylandSurface *Shell::surfaceAt(const Layer &layer, const QPointF &point, QPointF *local)
{
    // Iterate through the layer in reverse order
    for (int i = layer.size() - 1; i >= 0; --i) {
        QWaylandSurface *surface = layer.at(i);

        // Ignore hidden surfaces
        if (!surface->visible())
            continue;

        // Are the coordinates inside an item?
        QRectF geo(surface->pos(), surface->size());
        if (geo.contains(point)) {
            if (local)
                *local = point - surface->pos();
            return surface;
        }
    }

    return 0;
}

void Shell::addSurfaceToLayer(ShellWindowRole role, QWaylandSurface *surface)
{
    if (!surface)
        return;

    surface->setWindowProperty(QStringLiteral("role"), role);

    switch (role) {
    case Shell::BackgroundWindowRole:
        m_backgroundLayer.append(surface);
        break;
    case Shell::PanelWindowRole:
    case Shell::LauncherWindowRole:
    case Shell::SpecialWindowRole:
    case Shell::PopupWindowRole:
        m_panelsLayer.append(surface);
        break;
    case Shell::OverlayWindowRole:
        m_overlayLayer.append(surface);
        break;
    case Shell::DialogWindowRole:
        m_dialogsLayer.append(surface);
        break;
    default:
        break;
    }

    connect(surface, SIGNAL(unmapped()),
            this, SLOT(surfaceUnmapped()));
}

void Shell::hawaii_shell_bind_resource(Resource *resource)
{
    send_loaded(resource->handle);
}

void Shell::hawaii_shell_set_background(Resource *resource,
                                        struct ::wl_resource *output_resource,
                                        struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::BackgroundWindowRole, surface);
}

void Shell::hawaii_shell_set_panel(Resource *resource,
                                   struct ::wl_resource *output_resource,
                                   struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::PanelWindowRole, surface);
}

void Shell::hawaii_shell_set_launcher(Resource *resource,
                                      struct ::wl_resource *output_resource,
                                      struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::LauncherWindowRole, surface);
}

void Shell::hawaii_shell_set_special(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::SpecialWindowRole, surface);
}

void Shell::hawaii_shell_set_overlay(Resource *resource,
                                     struct ::wl_resource *output_resource,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::OverlayWindowRole, surface);
}

void Shell::hawaii_shell_set_popup(Resource *resource,
                                   uint32_t id,
                                   struct ::wl_resource *output_resource,
                                   struct ::wl_resource *surface_resource,
                                   int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), QPointF(x, y));
    addSurfaceToLayer(Shell::PopupWindowRole, surface);
}

void Shell::hawaii_shell_set_dialog(Resource *resource,
                                    struct ::wl_resource *output_resource,
                                    struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Shell::DialogWindowRole, surface);
}

void Shell::hawaii_shell_set_position(Resource *resource,
                                      struct ::wl_resource *surface_resource,
                                      int32_t x, int32_t y)
{
    Q_UNUSED(resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), QPointF(x, y));
    if (surface->surfaceItem())
        surface->surfaceItem()->setPosition(QPointF(x, y));
}

void Shell::hawaii_shell_desktop_ready(Resource *resource)
{
    Q_UNUSED(resource);
    Q_EMIT ready();
}

void Shell::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    switch (surface->windowProperties().value(QStringLiteral("role")).toInt()) {
    case Shell::BackgroundWindowRole:
        m_backgroundLayer.removeOne(surface);
        break;
    case Shell::PanelWindowRole:
    case Shell::LauncherWindowRole:
    case Shell::SpecialWindowRole:
    case Shell::PopupWindowRole:
        m_panelsLayer.removeOne(surface);
        break;
    case Shell::OverlayWindowRole:
        m_overlayLayer.removeOne(surface);
        break;
    case Shell::DialogWindowRole:
        m_dialogsLayer.removeOne(surface);
        break;
    default:
        break;
    }
}

#include "moc_shell.cpp"
