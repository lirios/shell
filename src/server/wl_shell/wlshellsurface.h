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

#ifndef WLSHELLSURFACE_H
#define WLSHELLSURFACE_H

#include <wayland-server.h>

#include "interface.h"
#include "shellsignal.h"

struct wl_resource;
struct wl_client;
struct wl_event_source;

class WlShell;
class ShellSurface;

class WlShellSurface : public Interface
{
public:
    WlShellSurface(WlShell *wlShell);
    ~WlShellSurface();

    ShellSurface *shsurf();
    inline wl_resource *resource() const { return m_resource; }

    void init(wl_client *client, uint32_t id);
    void ping(uint32_t serial);
    bool isResponsive() const;

    Signal<WlShellSurface *> responsivenessChangedSignal;

private:
    void pong(wl_client *client, wl_resource *resource, uint32_t serial);
    void move(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial);
    void resize(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial, uint32_t edges);
    void setToplevel(wl_client *client, wl_resource *resource);
    void setTransient(wl_client *client, wl_resource *resource, wl_resource *parent_resource, int x, int y, uint32_t flags);
    void setFullscreen(wl_client *client, wl_resource *resource, uint32_t method, uint32_t framerate, wl_resource *output_resource);
    void setPopup(wl_client *client, wl_resource *resource, wl_resource *seat_resource, uint32_t serial, wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags);
    void setMaximized(wl_client *client, wl_resource *resource, wl_resource *output_resource);
    void setTitle(wl_client *client, wl_resource *resource, const char *title);
    void setClass(wl_client *client, wl_resource *resource, const char *className);

    void resourceDestroyed();
    void destroyPingTimer();
    void pingTimeout();
    void popupDone();

    WlShell *m_wlShell;
    wl_resource *m_resource;

    struct PingTimer {
        wl_event_source *source;
        uint32_t serial;
    };
    PingTimer *m_pingTimer;
    bool m_unresponsive;

    static const struct wl_shell_surface_interface s_shellSurfaceImplementation;
};

#endif
