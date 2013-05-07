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

#ifndef SHELLSEAT_H
#define SHELLSEAT_H

#include <list>

#include <wayland-server.h>

#include "signal.h"

class ShellSurface;

class ShellSeat {
public:
    ~ShellSeat();
    static ShellSeat *shellSeat(struct weston_seat *seat);

    bool addPopupGrab(ShellSurface *surface, uint32_t serial);
    void removePopupGrab(ShellSurface *surface);
    void endPopupGrab();

    Signal<ShellSeat *, struct wl_pointer *> pointerFocusSignal;

private:
    ShellSeat(struct weston_seat *seat);
    static void seatDestroyed(struct wl_listener *listener, void *data);
    static void pointerFocus(struct wl_listener *listener, void *data);

    struct weston_seat *m_seat;
    struct Wrapper {
        ShellSeat *seat;
        struct wl_listener seatDestroy;
        struct wl_listener focus;
    } m_listeners;

    struct PopupGrab {
        ShellSeat *seat;
        struct wl_pointer_grab grab;
        std::list<ShellSurface *> surfaces;
        struct wl_client *client;
        int32_t initial_up;
    } m_popupGrab;

    static void popup_grab_focus(struct wl_pointer_grab *grab, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y);
    static void popup_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w);
    static const struct wl_pointer_grab_interface popup_grab_interface;
};

#endif
