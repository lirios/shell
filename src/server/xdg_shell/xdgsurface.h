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

#ifndef XDGSURFACE_H
#define XDGSURFACE_H

#include <wayland-server.h>

#include "interface.h"
#include "shellsignal.h"

struct wl_resource;
struct wl_client;
struct wl_event_source;

struct weston_output;

class XdgShell;
class ShellSurface;

class XdgBaseSurface : public Interface
{
public:
    XdgBaseSurface(XdgShell *xdgShell);
    virtual ~XdgBaseSurface();

    ShellSurface *shsurf();
    inline wl_resource *resource() const { return m_resource; }

    void ping(uint32_t serial);
    bool isResponsive() const;

    Signal<XdgBaseSurface *> responsivenessChangedSignal;

protected:
    virtual void sendPing(uint32_t serial) {}
    void pong(uint32_t serial);
    void resourceDestroyed();

    wl_resource *m_resource;

private:
    void destroyPingTimer();
    void pingTimeout();

    struct PingTimer {
        wl_event_source *source;
        uint32_t serial;
    };

    XdgShell *m_xdgShell;
    PingTimer *m_pingTimer;
    bool m_unresponsive;
};

class XdgSurface : public XdgBaseSurface
{
public:
    XdgSurface(XdgShell *xdgShell);

    void init(wl_client *client, uint32_t id);
    void gainFocus();
    void loseFocus();
    void sendPing(uint32_t serial) override;

private:
    void destroy(wl_client *client, wl_resource *resource);
    void setTransientFor(wl_client *client, wl_resource *resource, wl_resource *surface);
    void setTitle(wl_client *client, wl_resource *resource, const char *title);
    void setAppId(wl_client *client, wl_resource *resource, const char *id);
    void move(wl_client *client, wl_resource *resource, wl_resource *seat, uint32_t serial);
    void resize(wl_client *client, wl_resource *resource, wl_resource *seat, uint32_t serial, uint32_t edges);
    void setOutput(wl_client *client, wl_resource *resource, wl_resource *output_resource);
    void setFullscreen(wl_client *client, wl_resource *resource);
    void unsetFullscreen(wl_client *client, wl_resource *resource);
    void setMaximized(wl_client *client, wl_resource *resource);
    void unsetMaximized(wl_client *client, wl_resource *resource);
    void setMinimized(wl_client *client, wl_resource *resource);

    weston_output *m_output;
    int m_focus;

    static const struct xdg_surface_interface s_implementation;
};

class XdgPopup : public XdgBaseSurface
{
public:
    XdgPopup(XdgShell *xdgShell, uint32_t serial);

    void init(wl_client *client, uint32_t id);
    void sendPing(uint32_t serial) override;

private:
    void destroy(wl_client *client, wl_resource *resource);
    void popupDone();

    uint32_t m_serial;

    static const struct xdg_popup_interface s_implementation;
};

#endif
