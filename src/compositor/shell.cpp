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

#include <private/qwlsurface_p.h>

#include "shell.h"
#include "compositor.h"

Shell::Shell(struct ::wl_display *display)
    : QtWaylandServer::wl_hawaii_shell(display)
{
}

bool Shell::hasSurface(QWaylandSurface *surface) const
{
    return m_surfaces.contains(surface);
}

void Shell::hawaii_shell_bind_resource(Resource *resource)
{
    send_loaded(resource->handle);
}

void Shell::hawaii_shell_set_background(Resource *resource,
                                        struct ::wl_resource *output,
                                        struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("role"), Compositor::BackgroundWindowRole);
    surface->setWindowProperty(QStringLiteral("viewPosition"), surface->pos());
    m_surfaces.append(surface);
}

void Shell::hawaii_shell_set_panel(Resource *resource,
                                   struct ::wl_resource *output,
                                   struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("role"), Compositor::PanelWindowRole);
    surface->setWindowProperty(QStringLiteral("viewPosition"), surface->pos());
    m_surfaces.append(surface);
}

void Shell::hawaii_shell_set_launcher(Resource *resource,
                                      struct ::wl_resource *output,
                                      struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("role"), Compositor::LauncherWindowRole);
    surface->setWindowProperty(QStringLiteral("viewPosition"), surface->pos());
    m_surfaces.append(surface);
}

void Shell::hawaii_shell_set_special(Resource *resource,
                                     struct ::wl_resource *output,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("role"), Compositor::SpecialWindowRole);
    surface->setWindowProperty(QStringLiteral("viewPosition"), surface->pos());
    m_surfaces.append(surface);
}

void Shell::hawaii_shell_set_overlay(Resource *resource,
                                     struct ::wl_resource *output,
                                     struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("role"), Compositor::OverlayWindowRole);
    surface->setWindowProperty(QStringLiteral("viewPosition"), surface->pos());
    m_surfaces.append(surface);
}

void Shell::hawaii_shell_set_position(Resource *resource,
                                      struct ::wl_resource *surface_resource,
                                      int32_t x, int32_t y)
{
    Q_UNUSED(resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("viewPosition"), QPointF(x, y));
}

void Shell::hawaii_shell_desktop_ready(Resource *resource)
{
}
