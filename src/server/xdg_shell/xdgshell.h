/*
 * Copyright 2014 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#ifndef XDGSHELL_H
#define XDGSHELL_H

#include <wayland-server.h>

#include "shellsignal.h"
#include "interface.h"

struct weston_surface;
struct weston_seat;
struct weston_pointer;
struct weston_keyboard;

class ShellSurface;
class ShellSeat;
class XdgBaseSurface;

class XdgShell : public Interface
{
public:
    XdgShell();

    Signal<ShellSurface *, bool> surfaceResponsivenessChangedSignal;

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);
    void useUnstableVersion(wl_client *client, wl_resource *resource, int32_t version);
    ShellSurface *getXdgSurface(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource);
    void getXdgPopup(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource, wl_resource *parent,
                     wl_resource *seat, uint32_t serial, int32_t x, int32_t y, uint32_t flags);

    void pointerFocus(ShellSeat *seat, weston_pointer *pointer);
    void keyboardFocus(ShellSeat *seat, weston_keyboard *keyboard);
    void surfaceResponsiveness(XdgBaseSurface *shsurf);

    static void sendConfigure(weston_surface *surface, int32_t width, int32_t height);

    static const struct xdg_shell_interface shell_implementation;
    static const struct weston_shell_client shell_client;
};

#endif
