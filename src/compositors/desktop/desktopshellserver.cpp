/****************************************************************************
 * This file is part of Green Island.
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

#include <QtCompositor/private/qwlsurface_p.h>
#include <QtCompositor/QWaylandSurface>

#include "desktopshellserver.h"
#include "compositor.h"

const struct hawaii_desktop_shell_interface DesktopShellServer::shell_interface = {
    DesktopShellServer::set_available_geometry,
    DesktopShellServer::set_background,
    DesktopShellServer::set_panel,
    DesktopShellServer::set_panel_geometry,
    DesktopShellServer::set_launcher,
    DesktopShellServer::set_launcher_geometry,
    DesktopShellServer::set_special,
    DesktopShellServer::set_position,
    DesktopShellServer::hide_surface,
    DesktopShellServer::set_lock_surface,
    DesktopShellServer::unlock,
    DesktopShellServer::set_grab_surface
};

DesktopShellServer::DesktopShellServer(DesktopCompositor *compositor, QtWayland::Compositor *handle)
    : backgroundSurface(0)
    , panelSurface(0)
    , lockSurface(0)
    , m_compositor(compositor)
    , m_compositorHandle(handle)
{
    wl_display_add_global(handle->wl_display(),
                          &hawaii_desktop_shell_interface, this,
                          DesktopShellServer::bind_func);
}

DesktopShellServer::~DesktopShellServer()
{
}

void DesktopShellServer::bind_func(wl_client *client, void *data, uint32_t version, uint32_t id)
{
    Q_UNUSED(version);

    wl_resource *resource = wl_client_add_object(client,
                                                 &hawaii_desktop_shell_interface,
                                                 &shell_interface, id, data);
    resource->destroy = destroy_resource;

    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->m_resources.append(resource);
}

void DesktopShellServer::destroy_resource(wl_resource *resource)
{
    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->m_resources.removeOne(resource);
    free(resource);
}

void DesktopShellServer::set_available_geometry(struct wl_client *client,
                                                struct wl_resource *resource,
                                                struct wl_resource *output_resource,
                                                int32_t x, int32_t y,
                                                int32_t width, int32_t height)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(width);
    Q_UNUSED(height);
}

void DesktopShellServer::set_background(struct wl_client *client,
                                        struct wl_resource *resource,
                                        struct wl_resource *output,
                                        struct wl_resource *surface)
{
    Q_UNUSED(client);
    Q_UNUSED(output);

    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->backgroundSurface = QtWayland::Surface::fromResource(surface);

    hawaii_desktop_shell_send_present(resource, surface);
}

void DesktopShellServer::set_panel(struct wl_client *client,
                                   struct wl_resource *resource,
                                   struct wl_resource *output,
                                   struct wl_resource *surface)
{
    Q_UNUSED(client);
    Q_UNUSED(output);

    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->panelSurface = QtWayland::Surface::fromResource(surface);

    hawaii_desktop_shell_send_present(resource, surface);
}

void DesktopShellServer::set_panel_geometry(struct wl_client *client,
                                            struct wl_resource *resource,
                                            struct wl_resource *output,
                                            struct wl_resource *surface_resource,
                                            int32_t x, int32_t y,
                                            int32_t width, int32_t height)
{
    Q_UNUSED(client);
    Q_UNUSED(output);

    QtWayland::Surface *surface = QtWayland::Surface::fromResource(surface_resource);
    surface->setPos(QPointF(x, y));
    surface->setSize(QSize(width, height));
}

void DesktopShellServer::set_launcher(struct wl_client *client,
                                      struct wl_resource *resource,
                                      struct wl_resource *output,
                                      struct wl_resource *surface)
{
    Q_UNUSED(client);
    Q_UNUSED(output);

    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->launcherSurface = QtWayland::Surface::fromResource(surface);

    hawaii_desktop_shell_send_present(resource, surface);
}

void DesktopShellServer::set_launcher_geometry(struct wl_client *client,
                                               struct wl_resource *resource,
                                               struct wl_resource *output,
                                               struct wl_resource *surface_resource,
                                               int32_t x, int32_t y,
                                               int32_t width, int32_t height)
{
    Q_UNUSED(client);
    Q_UNUSED(output);

    QtWayland::Surface *surface = QtWayland::Surface::fromResource(surface_resource);
    surface->setPos(QPointF(x, y));
    surface->setSize(QSize(width, height));
}

void DesktopShellServer::set_special(struct wl_client *client,
                                     struct wl_resource *resource,
                                     struct wl_resource *output_resource,
                                     struct wl_resource *surface_resource)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QtWayland::Surface *surface = QtWayland::Surface::fromResource(surface_resource);
    Q_UNUSED(surface);
}

void DesktopShellServer::set_position(struct wl_client *client,
                                      struct wl_resource *resource,
                                      struct wl_resource *surface_resource,
                                      int32_t x, int32_t y)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);

    QtWayland::Surface *surface = QtWayland::Surface::fromResource(surface_resource);
    surface->setPos(QPointF(x, y));
}

void DesktopShellServer::hide_surface(struct wl_client *client,
                                      struct wl_resource *resource,
                                      struct wl_resource *surface_resource)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);

    QtWayland::Surface *surface = QtWayland::Surface::fromResource(surface_resource);
    Q_UNUSED(surface);
}

void DesktopShellServer::set_lock_surface(struct wl_client *client,
                                          struct wl_resource *resource,
                                          struct wl_resource *surface)
{
    DesktopShellServer *self = static_cast<DesktopShellServer *>(resource->data);
    self->lockSurface = QtWayland::Surface::fromResource(surface);
}

void DesktopShellServer::unlock(struct wl_client *client,
                                struct wl_resource *resource)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);
}

void DesktopShellServer::set_grab_surface(struct wl_client *client,
                                          struct wl_resource *resource,
                                          struct wl_resource *surface)
{
    Q_UNUSED(client);
    Q_UNUSED(resource);
    Q_UNUSED(surface);
}
