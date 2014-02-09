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

#include "xdgsurface.h"
#include "utils.h"
#include "shellsurface.h"
#include "shell.h"
#include "wayland-xdg-shell-server-protocol.h"

XdgBaseSurface::XdgBaseSurface(XdgShell *ws)
          : m_resource(nullptr)
          , m_xdgShell(ws)
          , m_pingTimer(nullptr)
          , m_unresponsive(false)
{
}

XdgBaseSurface::~XdgBaseSurface()
{
    destroyPingTimer();
    if (m_resource && wl_resource_get_client(m_resource)) {
        wl_resource_set_destructor(m_resource, nullptr);
        wl_resource_destroy(m_resource);
    }
}

void XdgBaseSurface::resourceDestroyed()
{
    m_resource = nullptr;
    object()->destroy();
}

ShellSurface *XdgBaseSurface::shsurf()
{
    return static_cast<ShellSurface *>(object());
}

bool XdgBaseSurface::isResponsive() const
{
    return !m_unresponsive;
}

void XdgBaseSurface::ping(uint32_t serial)
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
                                                     { static_cast<XdgBaseSurface *>(data)->pingTimeout(); return 1; }, this);
        wl_event_source_timer_update(m_pingTimer->source, ping_timeout);

        wl_shell_surface_send_ping(m_resource, serial);
    }
}

void XdgBaseSurface::pong(uint32_t serial)
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

void XdgBaseSurface::destroyPingTimer()
{
    if (m_pingTimer && m_pingTimer->source)
        wl_event_source_remove(m_pingTimer->source);

    delete m_pingTimer;
    m_pingTimer = nullptr;
}

void XdgBaseSurface::pingTimeout()
{
    if (!m_unresponsive) {
        m_unresponsive = true;
        responsivenessChangedSignal(this);
    }
}



XdgSurface::XdgSurface(XdgShell *ws)
          : XdgBaseSurface(ws)
          , m_output(nullptr)
          , m_focus(0)
{
}

void XdgSurface::init(wl_client *client, uint32_t id)
{
    m_resource = wl_resource_create(client, &xdg_surface_interface, 1, id);
    wl_resource_set_implementation(m_resource, &s_implementation, this, [](wl_resource *resource) { static_cast<XdgSurface *>(wl_resource_get_user_data(resource))->resourceDestroyed(); });
}

void XdgSurface::sendPing(uint32_t serial)
{
    xdg_surface_send_ping(m_resource, serial);
}

void XdgSurface::gainFocus()
{
    if (!m_focus++) {
        xdg_surface_send_focused_set(m_resource);
    }
}

void XdgSurface::loseFocus()
{
    if (!--m_focus) {
        xdg_surface_send_focused_unset(m_resource);
    }
}

void XdgSurface::destroy(wl_client *client, wl_resource *resource)
{
    wl_resource_destroy(m_resource);
}

void XdgSurface::setTransientFor(wl_client *client, wl_resource *resource, wl_resource *surface)
{
}

void XdgSurface::setTitle(wl_client *client, wl_resource *resource, const char *title)
{
    shsurf()->setTitle(title);
}

void XdgSurface::setAppId(wl_client *client, wl_resource *resource, const char *id)
{
    shsurf()->setClass(id);
}

void XdgSurface::move(wl_client *client, wl_resource *resource, wl_resource *seat, uint32_t serial)
{
    weston_seat *ws = static_cast<weston_seat *>(wl_resource_get_user_data(seat));
    weston_surface *surface = weston_surface_get_main_surface(ws->pointer->focus->surface);
    if (ws->pointer->button_count == 0 || ws->pointer->grab_serial != serial || surface != shsurf()->weston_surface()) {
        return;
    }

    shsurf()->dragMove(ws);
}

void XdgSurface::resize(wl_client *client, wl_resource *resource, wl_resource *seat, uint32_t serial, uint32_t edges)
{
    weston_seat *ws = static_cast<weston_seat *>(wl_resource_get_user_data(seat));
    weston_surface *surface = weston_surface_get_main_surface(ws->pointer->focus->surface);
    if (ws->pointer->button_count == 0 || ws->pointer->grab_serial != serial || surface != shsurf()->weston_surface()) {
        return;
    }

    shsurf()->dragResize(ws, (ShellSurface::Edges)edges);
}

void XdgSurface::setOutput(wl_client *client, wl_resource *resource, wl_resource *output_resource)
{
    m_output = output_resource ? static_cast<weston_output *>(wl_resource_get_user_data(output_resource)) : NULL;
}

void XdgSurface::setFullscreen(wl_client *client, wl_resource *resource)
{
    if (shsurf()->type() != ShellSurface::Type::TopLevel || shsurf()->isFullscreen()) {
        return;
    }

    shsurf()->setFullscreen(ShellSurface::FullscreenMethod::Default, 0, m_output);
}

void XdgSurface::unsetFullscreen(wl_client *client, wl_resource *resource)
{
    shsurf()->unsetFullscreen();
}

void XdgSurface::setMaximized(wl_client *client, wl_resource *resource)
{
    if (shsurf()->type() != ShellSurface::Type::TopLevel || shsurf()->isMaximized()) {
        return;
    }

    weston_output *output;
    if (weston_output *o = shsurf()->output()) {
        output = o;
    } else {
        output = Shell::instance()->getDefaultOutput();
    }

    shsurf()->setMaximized(output);
}

void XdgSurface::unsetMaximized(wl_client *client, wl_resource *resource)
{
    shsurf()->unsetMaximized();
}

void XdgSurface::setMinimized(wl_client *client, wl_resource *resource)
{
    shsurf()->setMinimized(true);
}

const struct xdg_surface_interface XdgSurface::s_implementation = {
    wrapInterface(&XdgSurface::destroy),
    wrapInterface(&XdgSurface::setTransientFor),
    wrapInterface(&XdgSurface::setTitle),
    wrapInterface(&XdgSurface::setAppId),
    wrapInterface(&XdgSurface::pong),
    wrapInterface(&XdgSurface::move),
    wrapInterface(&XdgSurface::resize),
    wrapInterface(&XdgSurface::setOutput),
    wrapInterface(&XdgSurface::setFullscreen),
    wrapInterface(&XdgSurface::unsetFullscreen),
    wrapInterface(&XdgSurface::setMaximized),
    wrapInterface(&XdgSurface::unsetMaximized),
    wrapInterface(&XdgSurface::setMinimized)
};



XdgPopup::XdgPopup(XdgShell *ws, uint32_t serial)
        : XdgBaseSurface(ws)
        , m_serial(serial)
{
}

void XdgPopup::init(wl_client *client, uint32_t id)
{
    m_resource = wl_resource_create(client, &xdg_popup_interface, 1, id);
    wl_resource_set_implementation(m_resource, &s_implementation, this, [](wl_resource *resource) { static_cast<XdgPopup *>(wl_resource_get_user_data(resource))->resourceDestroyed(); });

    shsurf()->popupDoneSignal.connect(this, &XdgPopup::popupDone);
}

void XdgPopup::sendPing(uint32_t serial)
{
    xdg_popup_send_ping(m_resource, serial);
}

void XdgPopup::popupDone()
{
    xdg_popup_send_popup_done(m_resource, m_serial);
}

void XdgPopup::destroy(wl_client *client, wl_resource *resource)
{
    wl_resource_destroy(m_resource);
}

const struct xdg_popup_interface XdgPopup::s_implementation = {
    wrapInterface(&XdgPopup::destroy),
    wrapInterface(&XdgPopup::pong)
};