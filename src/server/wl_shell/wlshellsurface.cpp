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

#include "wlshellsurface.h"
#include "shell.h"
#include "shellsurface.h"

WlShellSurface::WlShellSurface(WlShell *ws)
              : m_wlShell(ws)
              , m_pingTimer(nullptr)
              , m_unresponsive(false)
{
}

WlShellSurface::~WlShellSurface()
{
    destroyPingTimer();
    if (m_resource && wl_resource_get_client(m_resource)) {
        wl_resource_set_destructor(m_resource, nullptr);
        wl_resource_destroy(m_resource);
    }
}

void WlShellSurface::init(wl_client *client, uint32_t id)
{
    m_resource = wl_resource_create(client, &wl_shell_surface_interface, 1, id);
    wl_resource_set_implementation(m_resource, &s_shellSurfaceImplementation, this, [](wl_resource *resource) { static_cast<WlShellSurface *>(wl_resource_get_user_data(resource))->resourceDestroyed(); });

    shsurf()->popupDoneSignal.connect(this, &WlShellSurface::popupDone);
}

ShellSurface *WlShellSurface::shsurf()
{
    return static_cast<ShellSurface *>(object());
}

void WlShellSurface::resourceDestroyed()
{
    m_resource = nullptr;
    object()->destroy();
}

void WlShellSurface::ping(uint32_t serial)
{
    const int ping_timeout = 200;

    if (!m_resource || !wl_resource_get_client(m_resource))
        return;

    if (!m_pingTimer) {
        m_pingTimer = new PingTimer;
        if (!m_pingTimer)
            return;

        m_pingTimer->serial = serial;
        wl_event_loop *loop = wl_display_get_event_loop(Shell::compositor()->wl_display);
        m_pingTimer->source = wl_event_loop_add_timer(loop, [](void *data)
                                                     { static_cast<WlShellSurface *>(data)->pingTimeout(); return 1; }, this);
        wl_event_source_timer_update(m_pingTimer->source, ping_timeout);

        wl_shell_surface_send_ping(m_resource, serial);
    }
}

bool WlShellSurface::isResponsive() const
{
    return !m_unresponsive;
}

void WlShellSurface::destroyPingTimer()
{
    if (m_pingTimer && m_pingTimer->source)
        wl_event_source_remove(m_pingTimer->source);

    delete m_pingTimer;
    m_pingTimer = nullptr;
}

void WlShellSurface::pingTimeout()
{
    if (!m_unresponsive) {
        m_unresponsive = true;
        responsivenessChangedSignal(this);
    }
}

void WlShellSurface::popupDone()
{
    wl_shell_surface_send_popup_done(m_resource);
}

void WlShellSurface::pong(wl_client *client, wl_resource *resource, uint32_t serial)
{
    if (!m_pingTimer)
        /* Just ignore unsolicited pong. */
        return;

    if (m_pingTimer->serial == serial) {
        destroyPingTimer();
        if (m_unresponsive) {
            m_unresponsive = false;
            responsivenessChangedSignal(this);
        }
    }
}

void WlShellSurface::move(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial)
{
    weston_seat *ws = static_cast<weston_seat *>(wl_resource_get_user_data(seat_resource));
    weston_surface *surface = weston_surface_get_main_surface(ws->pointer->focus->surface);
    if (ws->pointer->button_count == 0 || ws->pointer->grab_serial != serial || surface != shsurf()->weston_surface()) {
        return;
    }

    shsurf()->dragMove(ws);
}

void WlShellSurface::resize(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial, uint32_t edges)
{
    weston_seat *ws = static_cast<weston_seat *>(wl_resource_get_user_data(seat_resource));
    weston_surface *surface = weston_surface_get_main_surface(ws->pointer->focus->surface);
    if (ws->pointer->button_count == 0 || ws->pointer->grab_serial != serial || surface != shsurf()->weston_surface()) {
        return;
    }

    shsurf()->dragResize(ws, (ShellSurface::Edges)edges);
}

void WlShellSurface::setToplevel(wl_client *, wl_resource *)
{
    shsurf()->setTopLevel();
}

void WlShellSurface::setTransient(wl_client *client, wl_resource *resource, wl_resource *parent_resource, int x, int y, uint32_t flags)
{
    shsurf()->setTransient(static_cast<weston_surface *>(wl_resource_get_user_data(parent_resource)), x, y, flags & WL_SHELL_SURFACE_TRANSIENT_INACTIVE);
}

void WlShellSurface::setFullscreen(wl_client *client, wl_resource *resource, uint32_t method, uint32_t framerate, wl_resource *output_resource)
{
    weston_output *output = output_resource ? static_cast<weston_output *>(wl_resource_get_user_data(output_resource)) : nullptr;
    shsurf()->setFullscreen((ShellSurface::FullscreenMethod)method, framerate, output);
}

void WlShellSurface::setPopup(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial, wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags)
{
    weston_surface *parent = static_cast<weston_surface *>(wl_resource_get_user_data(parent_resource));
    weston_seat *seat = static_cast<weston_seat *>(wl_resource_get_user_data(seat_resource));
    shsurf()->setPopup(parent, seat, x, y, serial);
}

void WlShellSurface::setMaximized(wl_client *client, wl_resource *resource, wl_resource *output_resource)
{
    /* get the default output, if the client set it as NULL
     *   c heck whether the ouput is available */
    weston_output *output;
    if (output_resource) {
        output = static_cast<weston_output *>(wl_resource_get_user_data(output_resource));
    } else if (weston_output *o = shsurf()->output()) {
        output = o;
    } else {
        output = Shell::instance()->getDefaultOutput();
    }

    shsurf()->setMaximized(output);
}

void WlShellSurface::setTitle(wl_client *client, wl_resource *resource, const char *title)
{
    shsurf()->setTitle(title);
}

void WlShellSurface::setClass(wl_client *client, wl_resource *resource, const char *className)
{
    shsurf()->setClass(className);
}

const struct wl_shell_surface_interface WlShellSurface::s_shellSurfaceImplementation = {
    wrapInterface(&WlShellSurface::pong),
    wrapInterface(&WlShellSurface::move),
    wrapInterface(&WlShellSurface::resize),
    wrapInterface(&WlShellSurface::setToplevel),
    wrapInterface(&WlShellSurface::setTransient),
    wrapInterface(&WlShellSurface::setFullscreen),
    wrapInterface(&WlShellSurface::setPopup),
    wrapInterface(&WlShellSurface::setMaximized),
    wrapInterface(&WlShellSurface::setTitle),
    wrapInterface(&WlShellSurface::setClass)
};
