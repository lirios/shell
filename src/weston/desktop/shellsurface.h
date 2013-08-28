/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#ifndef SHELL_SURFACE_H
#define SHELL_SURFACE_H

#include <string>

#include <wayland-server.h>

#include "signal.h"
#include "utils.h"

class Shell;
class ShellSeat;
class Workspace;
class ShellGrab;

class ShellSurface {
public:
    enum class Type {
        None,
        TopLevel,
        Maximized,
        Transient,
        Popup,
        Fullscreen,
        XWayland
    };
    ShellSurface(Shell *shell, struct weston_surface *surface);
    ~ShellSurface();

    void init(struct wl_client *client, uint32_t id);
    bool updateType();
    void map(int32_t x, int32_t y, int32_t width, int32_t height);
    void unmapped();
    void advertize();

    void setTopLevel();
    void setTransient(weston_surface *parent, int x, int y, uint32_t flags);
    void setFullscreen(uint32_t method, uint32_t framerate, weston_output *output);
    void setXWayland(int x, int y, uint32_t flags);

    void calculateInitialPosition(int &x, int &y);

    void addTransform(struct weston_transform *transform);
    void removeTransform(struct weston_transform *transform);
    void damage();
    void setAlpha(float alpha);

    inline Shell *shell() const { return m_shell; }
    inline struct wl_resource *wl_resource() { return m_resource; }
    inline const struct wl_resource *wl_resource() const { return m_resource; }
    inline struct wl_client *client() const { return wl_resource_get_client(m_surface->resource); }
    inline struct weston_surface *weston_surface() const { return m_surface; }

    inline Type type() const { return m_type; }
    bool isMapped() const;
    int32_t x() const;
    int32_t y() const;
    int32_t width() const;
    int32_t height() const;
    int32_t transformedWidth() const;
    int32_t transformedHeight() const;
    float alpha() const;
    inline bool is(struct weston_surface *s) const { return s == m_surface; }
    bool isPopup() const;
    inline struct weston_output *output() const { return m_surface->output; }
    ShellSurface *topLevelParent();
    inline Workspace *workspace() const { return m_workspace; }
    struct weston_surface *transformParent() const;

    inline std::string title() const { return m_title; }

    void dragMove(struct weston_seat *ws);
    void dragResize(struct weston_seat *ws, uint32_t edges);
    void popupDone();

    void ping(uint32_t serial);
    bool isResponsive() const;

    void setState(int state);
    void setActive(bool active);
    bool isActive() const;
    bool isMinimized() const;
    void setAcceptNewState(bool accept) { m_acceptState = accept; }

    void activate();
    void deactivate();
    void minimize();
    void unminimize();

    void show();
    void hide();

    Signal<> destroyedSignal;
    Signal<ShellSurface *> moveStartSignal;
    Signal<ShellSurface *> moveEndSignal;
    Signal<ShellSurface *> pingTimeoutSignal;
    Signal<ShellSurface *> pongSignal;
    Signal<ShellSurface *> minimizedSignal;
    Signal<ShellSurface *> unminimizedSignal;

private:
    void unsetFullscreen();
    void unsetMaximized();
    void mapPopup();
    void centerOnOutput(struct weston_output *output);
    void surfaceDestroyed();
    int pingTimeout();
    void destroyPingTimer();
    void destroyWindow();
    void sendState();

    Shell *m_shell;
    Workspace *m_workspace;
    struct wl_resource *m_resource;
    struct wl_resource *m_windowResource;
    struct weston_surface *m_surface;
    WlListener m_surfaceDestroyListener;
    Type m_type;
    Type m_pendingType;
    const struct weston_shell_client *m_client;
    std::string m_title;
    std::string m_class;
    struct weston_output *m_output;
    int32_t m_savedX, m_savedY;
    bool m_unresponsive;
    int32_t m_state;
    bool m_windowAdvertized;
    bool m_acceptState;
    ShellGrab *m_runningGrab;

    struct weston_surface *m_parent;
    struct {
        int32_t x, y;
        uint32_t flags;
    } m_transient;

    struct {
        int32_t x, y;
        uint32_t serial;
        ShellSeat *seat;
    } m_popup;

    struct {
        enum wl_shell_surface_fullscreen_method type;
        struct weston_transform transform; /* matrix from x, y */
        uint32_t framerate;
        struct weston_surface *blackSurface;
        struct weston_output *output;
    } m_fullscreen;

    struct PingTimer {
        struct wl_event_source *source;
        uint32_t serial;
    };
    PingTimer *m_pingTimer;

    void pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    void move(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                   uint32_t serial);
    void resize(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                     uint32_t serial, uint32_t edges);
    void setToplevel(struct wl_client *client, struct wl_resource *resource);
    void setTransient(struct wl_client *client, struct wl_resource *resource,
                                            struct wl_resource *parent_resource, int x, int y, uint32_t flags);
    void setFullscreen(struct wl_client *client, struct wl_resource *resource, uint32_t method,
                                             uint32_t framerate, struct wl_resource *output_resource);
    void setPopup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                        uint32_t serial, struct wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags);
    void setMaximized(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource);
    void setTitle(struct wl_client *client, struct wl_resource *resource, const char *title);
    void setClass(struct wl_client *client, struct wl_resource *resource, const char *className);

    static const struct wl_shell_surface_interface m_shell_surface_implementation;

    static void move_grab_motion(struct weston_pointer_grab *grab, uint32_t time);
    static void move_grab_button(struct weston_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w);
    static const struct weston_pointer_grab_interface m_move_grab_interface;

    static void resize_grab_motion(struct weston_pointer_grab *grab, uint32_t time);
    static void resize_grab_button(struct weston_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w);
    static const struct weston_pointer_grab_interface m_resize_grab_interface;

    static void set_state(struct wl_client *client, struct wl_resource *resource, int32_t state);
    static const struct hawaii_window_interface m_window_implementation;

    friend class Shell;
    friend class Layer;
    friend class Workspace;
};

#endif
