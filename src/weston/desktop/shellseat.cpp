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

#include <weston/compositor.h>

#include "shellseat.h"
#include "shellsurface.h"

ShellSeat::ShellSeat(struct weston_seat *seat)
         : m_seat(seat)
{
    m_popupGrab.client = nullptr;
    m_popupGrab.seat = this;
    m_listeners.seat = this;

    m_listeners.seatDestroy.notify = seatDestroyed;
    wl_signal_add(&seat->destroy_signal, &m_listeners.seatDestroy);

    if (seat->seat.pointer) {
        m_listeners.focus.notify = pointerFocus;
        wl_signal_add(&seat->seat.pointer->focus_signal, &m_listeners.focus);
    } else {
        wl_list_init(&m_listeners.focus.link);
    }
}

ShellSeat::~ShellSeat()
{
    if (m_popupGrab.client) {
        wl_pointer_end_grab(m_popupGrab.grab.pointer);
    }
    wl_list_remove(&m_listeners.seatDestroy.link);
    wl_list_remove(&m_listeners.focus.link);
}

ShellSeat *ShellSeat::shellSeat(struct weston_seat *seat)
{
    struct wl_listener *listener = wl_signal_get(&seat->destroy_signal, ShellSeat::seatDestroyed);
    if (!listener) {
        return new ShellSeat(seat);
    }

    return static_cast<Wrapper *>(container_of(listener, Wrapper, seatDestroy))->seat;
}

void ShellSeat::seatDestroyed(struct wl_listener *listener, void *data)
{
    ShellSeat *shseat = static_cast<Wrapper *>(container_of(listener, Wrapper, seatDestroy))->seat;
    delete shseat;
}

void ShellSeat::pointerFocus(struct wl_listener *listener, void *data)
{
    ShellSeat *shseat = static_cast<Wrapper *>(container_of(listener, Wrapper, focus))->seat;
    struct wl_pointer *pointer = static_cast<wl_pointer *>(data);
    shseat->pointerFocusSignal(shseat, pointer);
}

void ShellSeat::popup_grab_focus(struct wl_pointer_grab *grab, struct wl_surface *surface, wl_fixed_t x, wl_fixed_t y)
{
    struct wl_pointer *pointer = grab->pointer;
    ShellSeat *shseat = static_cast<PopupGrab *>(container_of(grab, PopupGrab, grab))->seat;

    if (surface && surface->resource.client == shseat->m_popupGrab.client) {
        wl_pointer_set_focus(pointer, surface, x, y);
        grab->focus = surface;
    } else {
        wl_pointer_set_focus(pointer, NULL, wl_fixed_from_int(0), wl_fixed_from_int(0));
        grab->focus = NULL;
    }
}

static void popup_grab_motion(struct wl_pointer_grab *grab,  uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    struct wl_resource *resource = grab->pointer->focus_resource;
    if (resource)
        wl_pointer_send_motion(resource, time, sx, sy);
}

void ShellSeat::popup_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w)
{
    ShellSeat *shseat = static_cast<PopupGrab *>(container_of(grab, PopupGrab, grab))->seat;

    struct wl_resource *resource = grab->pointer->focus_resource;
    if (resource) {
        struct wl_display *display = wl_client_get_display(resource->client);
        uint32_t serial = wl_display_get_serial(display);
        wl_pointer_send_button(resource, serial, time, button, state_w);
    } else if (state_w == WL_POINTER_BUTTON_STATE_RELEASED &&
              (shseat->m_popupGrab.initial_up || time - shseat->m_seat->pointer.grab_time > 500)) {
        shseat->endPopupGrab();
    }

    if (state_w == WL_POINTER_BUTTON_STATE_RELEASED)
        shseat->m_popupGrab.initial_up = 1;
}

const struct wl_pointer_grab_interface ShellSeat::popup_grab_interface = {
    ShellSeat::popup_grab_focus,
    popup_grab_motion,
    ShellSeat::popup_grab_button,
};

bool ShellSeat::addPopupGrab(ShellSurface *surface, uint32_t serial)
{
    if (serial == m_seat->pointer.grab_serial) {
        if (m_popupGrab.surfaces.empty()) {
            m_popupGrab.client = surface->client();
            m_popupGrab.grab.interface = &popup_grab_interface;
            /* We must make sure here that this popup was opened after
            * a mouse press, and not just by moving around with other
            * popups already open. */
            if (m_seat->pointer.button_count > 0) {
                m_popupGrab.initial_up = 0;
            }

            wl_pointer_start_grab(m_seat->seat.pointer, &m_popupGrab.grab);
        }
        m_popupGrab.surfaces.push_back(surface);

        return true;
    }

    m_popupGrab.client = nullptr;
    return false;
}

void ShellSeat::removePopupGrab(ShellSurface *surface)
{
    m_popupGrab.surfaces.remove(surface);
    if (m_popupGrab.surfaces.empty()) {
        wl_pointer_end_grab(m_popupGrab.grab.pointer);
        m_popupGrab.client = nullptr;
    }
}

void ShellSeat::endPopupGrab()
{
    struct wl_pointer *pointer = m_popupGrab.grab.pointer;
    if (m_popupGrab.client) {
        wl_pointer_end_grab(pointer->grab->pointer);
        m_popupGrab.client = nullptr;
        /* Send the popup_done event to all the popups open */
        for (ShellSurface *shsurf: m_popupGrab.surfaces) {
            shsurf->popupDone();
        }
        m_popupGrab.surfaces.clear();
    }
}
