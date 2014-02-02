/*
 * Copyright 2013  Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WLSHELL_H
#define WLSHELL_H

#include <wayland-server.h>

#include "shellsignal.h"
#include "interface.h"

struct weston_surface;
struct weston_seat;
struct weston_pointer;

class ShellSurface;
class ShellSeat;
class WlShellSurface;

class WlShell : public Interface
{
public:
    WlShell();

    Signal<ShellSurface *, bool> surfaceResponsivenessChangedSignal;

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);
    ShellSurface *getShellSurface(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource);
    void pointerFocus(ShellSeat *seat, weston_pointer *pointer);
    void surfaceResponsiveness(WlShellSurface *shsurf);

    static void sendConfigure(weston_surface *surface, uint32_t edges, int32_t width, int32_t height);

    static const struct wl_shell_interface shell_implementation;
    static const struct weston_shell_client shell_client;
};

#endif
