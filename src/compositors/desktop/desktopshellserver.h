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

#ifndef DESKTOPSHELLSERVER_H
#define DESKTOPSHELLSERVER_H

#include <QtCompositor/private/qwlcompositor_p.h>
#include <wayland-util.h>

#include "wayland-desktop-shell-server-protocol.h"
#include "compositor.h"

class DesktopShellServer
{
public:
    DesktopShellServer(DesktopCompositor *compositor,
                       QtWayland::Compositor *handle);
    ~DesktopShellServer();

    QtWayland::Surface *backgroundSurface;
    QtWayland::Surface *panelSurface;
    QtWayland::Surface *launcherSurface;
    QtWayland::Surface *lockSurface;

private:
    static void bind_func(struct wl_client *client, void *data,
                          uint32_t version, uint32_t id);

    static void destroy_resource(wl_resource *resource);

    static void set_available_geometry(struct wl_client *client,
                                       struct wl_resource *resource,
                                       struct wl_resource *output_resource,
                                       int32_t x, int32_t y,
                                       int32_t width, int32_t height);

    static void set_background(struct wl_client *client,
                               struct wl_resource *resource,
                               struct wl_resource *output,
                               struct wl_resource *surface);

    static void set_panel(struct wl_client *client,
                          struct wl_resource *resource,
                          struct wl_resource *output,
                          struct wl_resource *surface);
    static void set_panel_geometry(struct wl_client *client,
                                   struct wl_resource *resource,
                                   struct wl_resource *output,
                                   struct wl_resource *surface_resource,
                                   int32_t x, int32_t y,
                                   int32_t width, int32_t height);

    static void set_launcher(struct wl_client *client,
                             struct wl_resource *resource,
                             struct wl_resource *output,
                             struct wl_resource *surface);
    static void set_launcher_geometry(struct wl_client *client,
                                      struct wl_resource *resource,
                                      struct wl_resource *output,
                                      struct wl_resource *surface_resource,
                                      int32_t x, int32_t y,
                                      int32_t width, int32_t height);

    static void set_special(struct wl_client *client,
                            struct wl_resource *resource,
                            struct wl_resource *output_resource,
                            struct wl_resource *surface);
    static void set_position(struct wl_client *client,
                             struct wl_resource *resource,
                             struct wl_resource *surface,
                             int32_t x, int32_t y);
    static void hide_surface(struct wl_client *client,
                             struct wl_resource *resource,
                             struct wl_resource *surface);

    static void set_lock_surface(struct wl_client *client,
                                 struct wl_resource *resource,
                                 struct wl_resource *surface);

    static void unlock(struct wl_client *client,
                       struct wl_resource *resource);

    static void set_grab_surface(struct wl_client *client,
                                 struct wl_resource *resource,
                                 struct wl_resource *surface);

    static const struct hawaii_desktop_shell_interface shell_interface;

    DesktopCompositor *m_compositor;
    QtWayland::Compositor *m_compositorHandle;
    QList<wl_resource *> m_resources;
};

#endif // DESKTOPSHELLSERVER_H
