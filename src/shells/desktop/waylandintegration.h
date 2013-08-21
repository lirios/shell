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

#ifndef WAYLANDINTEGRATION_H
#define WAYLANDINTEGRATION_H

#include "wayland-desktop-shell-client-protocol.h"
#include "wayland-notification-daemon-client-protocol.h"

class WaylandIntegration
{
public:
    WaylandIntegration();

    static WaylandIntegration *instance();

    static const struct wl_registry_listener registryListener;

    static void handleGlobal(void *data,
                             wl_registry *registry,
                             uint32_t id,
                             const char *interface,
                             uint32_t version);

    /*
     * Desktop Shell Protocol
     */

    struct hawaii_desktop_shell *shell;
    static const struct hawaii_desktop_shell_listener shellListener;

    static void handleLoaded(void *data,
                             struct hawaii_desktop_shell *desktop_shell);

    static void handlePresent(void *data,
                              struct hawaii_desktop_shell *desktop_shell,
                              struct wl_surface *surface);

    static void handlePrepareLockSurface(void *data,
                                         struct hawaii_desktop_shell *desktop_shell);

    static void handleGrabCursor(void *data,
                                 struct hawaii_desktop_shell *desktop_shell,
                                 uint32_t cursor);

    static void handleWindowMapped(void *data,
                                   hawaii_desktop_shell *desktop_shell,
                                   hawaii_window *window,
                                   const char *title,
                                   const char *identifier,
                                   int32_t state);

    static void handleWorkspaceAdded(void *data,
                                     hawaii_desktop_shell *desktop_shell,
                                     hawaii_workspace *ws,
                                     int32_t num);

    /*
     * Notification Daemon Protocol
     */

    struct wl_notification_daemon *notification;
};

#endif // WAYLANDINTEGRATION_H
