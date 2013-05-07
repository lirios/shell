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

#ifndef SHELL_SURFACE_H
#define SHELL_SURFACE_H

#include <string>

#include <wayland-server.h>

#include "signal.h"
#include "utils.h"

class Shell;
class ShellSeat;
class Workspace;

class ShellSurface {
public:
    enum class Type {
        None,
        TopLevel,
        Maximized,
        Transient,
        Popup,
        Fullscreen
    };
    ShellSurface(Shell *shell, struct weston_surface *surface);
    ~ShellSurface();

    void init(uint32_t id, Workspace *workspace);
    bool updateType();
    void map(int32_t x, int32_t y, int32_t width, int32_t height);
    void unmapped();

    void addTransform(struct weston_transform *transform);
    void removeTransform(struct weston_transform *transform);
    void damage();
    void setAlpha(float alpha);

    inline Shell *shell() const { return m_shell; }
    inline struct wl_resource *wl_resource() { return &m_resource; }
    inline const struct wl_resource *wl_resource() const { return &m_resource; }
    inline struct wl_client *client() const { return m_surface->surface.resource.client; }
    inline struct wl_surface *wl_surface() const { return &m_surface->surface; }

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

    void dragMove(struct weston_seat *ws);
    void dragResize(struct weston_seat *ws, uint32_t edges);
    void popupDone();

    void ping(uint32_t serial);
    bool isResponsive() const;

    Signal<ShellSurface *> moveStartSignal;
    Signal<ShellSurface *> moveEndSignal;
    Signal<ShellSurface *> pingTimeoutSignal;
    Signal<ShellSurface *> pongSignal;

private:
    void setFullscreen(uint32_t method, uint32_t framerate, struct weston_output *output);
    void unsetFullscreen();
    void unsetMaximized();
    void mapPopup();
    void centerOnOutput(struct weston_output *output);
    void surfaceDestroyed();
    int pingTimeout();
    void destroyPingTimer();

    Shell *m_shell;
    Workspace *m_workspace;
    struct wl_resource m_resource;
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

    static void shell_surface_pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    static void shell_surface_move(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                   uint32_t serial);
    static void shell_surface_resize(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                     uint32_t serial, uint32_t edges);
    static void shell_surface_set_toplevel(struct wl_client *client, struct wl_resource *resource);
    static void shell_surface_set_transient(struct wl_client *client, struct wl_resource *resource,
                                            struct wl_resource *parent_resource, int x, int y, uint32_t flags);
    static void shell_surface_set_fullscreen(struct wl_client *client, struct wl_resource *resource, uint32_t method,
                                             uint32_t framerate, struct wl_resource *output_resource);
    static void shell_surface_set_popup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                        uint32_t serial, struct wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags);
    static void shell_surface_set_maximized(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource);
    static void shell_surface_set_title(struct wl_client *client, struct wl_resource *resource, const char *title);
    static void shell_surface_set_class(struct wl_client *client, struct wl_resource *resource, const char *className);
    static const struct wl_shell_surface_interface m_shell_surface_implementation;

    static void move_grab_motion(struct wl_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    static void move_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w);
    static const struct wl_pointer_grab_interface m_move_grab_interface;

    static void resize_grab_motion(struct wl_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    static void resize_grab_button(struct wl_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state_w);
    static const struct wl_pointer_grab_interface m_resize_grab_interface;

    friend class Shell;
    friend class Layer;
    friend class Workspace;
};

inline void ShellSurface::shell_surface_pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {
    static_cast<ShellSurface *>(resource->data)->pong(client, resource, serial);
}

inline void ShellSurface::shell_surface_move(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                             uint32_t serial) {
    static_cast<ShellSurface *>(resource->data)->move(client, resource, seat_resource, serial);
}

inline void ShellSurface::shell_surface_resize(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                 uint32_t serial, uint32_t edges) {
    static_cast<ShellSurface *>(resource->data)->resize(client, resource, seat_resource, serial, edges);
}

inline void ShellSurface::shell_surface_set_toplevel(struct wl_client *client, struct wl_resource *resource) {
    static_cast<ShellSurface *>(resource->data)->setToplevel(client, resource);
}

inline void ShellSurface::shell_surface_set_transient(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *parent_resource, int x, int y, uint32_t flags) {
    static_cast<ShellSurface *>(resource->data)->setTransient(client, resource, parent_resource, x, y, flags);
}

inline void ShellSurface::shell_surface_set_fullscreen(struct wl_client *client, struct wl_resource *resource, uint32_t method,
                                         uint32_t framerate, struct wl_resource *output_resource) {
    static_cast<ShellSurface *>(resource->data)->setFullscreen(client, resource, method, framerate, output_resource);
}

inline void ShellSurface::shell_surface_set_popup(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat_resource,
                                    uint32_t serial, struct wl_resource *parent_resource, int32_t x, int32_t y, uint32_t flags) {
    static_cast<ShellSurface *>(resource->data)->setPopup(client, resource, seat_resource, serial, parent_resource, x, y, flags);
}

inline void ShellSurface::shell_surface_set_maximized(struct wl_client *client, struct wl_resource *resource,
                                                      struct wl_resource *output_resource) {
    static_cast<ShellSurface *>(resource->data)->setMaximized(client, resource, output_resource);
}

inline void ShellSurface::shell_surface_set_title(struct wl_client *client, struct wl_resource *resource, const char *title) {
    static_cast<ShellSurface *>(resource->data)->setTitle(client, resource, title);
}

inline void ShellSurface::shell_surface_set_class(struct wl_client *client, struct wl_resource *resource, const char *className) {
    static_cast<ShellSurface *>(resource->data)->setClass(client, resource, className);
}

#endif
