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

#include "shellsignal.h"
#include "utils.h"
#include "interface.h"

struct weston_view;

class Shell;
class ShellSeat;
class Workspace;
class ShellGrab;

class ShellSurface : public Object {
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
    enum class Edges {
        None = 0,
        Top = 1,
        Bottom = 2,
        Left = 4,
        Right = 8,
        TopLeft = Top | Left,
        BottomLeft = Bottom | Left,
        TopRight = Top | Right,
        BottomRight = Bottom | Right
    };
    enum class FullscreenMethod {
        Default = 0,
        Scale = 1,
        Driver = 2,
        Fill = 3
    };
    ShellSurface(Shell *shell, struct weston_surface *surface);
    ~ShellSurface();

    bool updateType();
    void map(int32_t x, int32_t y);
    void unmapped();

    void setTopLevel();
    void setTransient(weston_surface *parent, int x, int y, bool inactive);
    void setFullscreen(FullscreenMethod method, uint32_t framerate, weston_output *output);
    void setXWayland(int x, int y, bool inactive);
    void setPopup(weston_surface *parent, weston_seat *seat, int32_t x, int32_t y, uint32_t serial);
    void setMaximized(weston_output *output);

    void addTransform(struct weston_transform *transform);
    void removeTransform(struct weston_transform *transform);
    void damage();
    void setAlpha(float alpha);

    inline Shell *shell() const { return m_shell; }
    inline struct wl_client *client() const { return wl_resource_get_client(m_surface->resource); }
    inline struct weston_surface *weston_surface() const { return m_surface; }
    inline weston_view *view() const { return m_view; }

    inline Type type() const { return m_type; }
    bool isMapped() const;
    int32_t x() const;
    int32_t y() const;
    int32_t width() const;
    int32_t height() const;
    float transformedX() const;
    float transformedY() const;
    int32_t transformedWidth() const;
    int32_t transformedHeight() const;
    void setPosition(float x, float y);
    IRect2D surfaceTreeBoundingBox() const;
    float alpha() const;
    inline bool is(struct weston_surface *s) const { return s == m_surface; }
    inline bool is(weston_view *v) const { return v->surface == m_surface; }
    bool isPopup() const;
    inline struct weston_output *output() const { return m_surface->output; }
    ShellSurface *topLevelParent();
    inline Workspace *workspace() const { return m_workspace; }
    weston_view *transformParent() const;

    void setTitle(const char *title);
    inline std::string title() const { return m_title; }
    void setClass(const char *c);

    void dragMove(struct weston_seat *ws);
    void dragResize(weston_seat *ws, Edges edges);
    void popupDone();

    void setActive(bool active);
    bool isActive() const;
    bool isMinimized() const;
    void setMinimized(bool min);
    void setAcceptNewState(bool accept) { m_acceptState = accept; }

    void activate();
    void deactivate();
    void minimize();
    void unminimize();

    void show();
    void hide();
    void close();

    Signal<> destroyedSignal;
    Signal<ShellSurface *> moveStartSignal;
    Signal<ShellSurface *> moveEndSignal;
    Signal<ShellSurface *> minimizedSignal;
    Signal<ShellSurface *> unminimizedSignal;
    Signal<> popupDoneSignal;
    Signal<> typeChangedSignal;
    Signal<> titleChangedSignal;
    Signal<> activeChangedSignal;
    Signal<> mappedSignal;
    Signal<> unmappedSignal;

private:
    void unsetFullscreen();
    void unsetMaximized();
    void mapPopup();
    void centerOnOutput(struct weston_output *output);
    void sendState();
    void destroy(void *data);
    void savePos();
    void restorePos();

    Shell *m_shell;
    Workspace *m_workspace;
    struct weston_surface *m_surface;
    weston_view *m_view;
    WlListener m_surfaceDestroyListener;
    Type m_type;
    Type m_pendingType;
    const struct weston_shell_client *m_client;
    std::string m_title;
    std::string m_class;
    struct weston_output *m_output;
    int32_t m_savedX, m_savedY;
    bool m_savedPos;
    bool m_acceptState;
    ShellGrab *m_runningGrab;
    int32_t m_lastWidth, m_lastHeight;
    bool m_active;
    bool m_minimized;

    struct weston_surface *m_parent;
    struct {
        int32_t x, y;
        bool inactive;
    } m_transient;

    struct {
        int32_t x, y;
        uint32_t serial;
        ShellSeat *seat;
    } m_popup;

    struct {
        FullscreenMethod type;
        struct weston_transform transform; /* matrix from x, y */
        uint32_t framerate;
        weston_view *blackView;
        struct weston_output *output;
    } m_fullscreen;

    friend class Shell;
    friend class Layer;
    friend class Workspace;
    friend class MoveGrab;
    friend class ResizeGrab;
};

inline bool operator&(ShellSurface::Edges a, ShellSurface::Edges b)
{
    return (int)a & (int)b;
}
inline ShellSurface::Edges operator|(ShellSurface::Edges a, ShellSurface::Edges b)
{
    return (ShellSurface::Edges)((int)a | (int)b);
}
inline ShellSurface::Edges &operator|=(ShellSurface::Edges &a, ShellSurface::Edges b)
{
    a = a | b;
    return a;
}

#endif
