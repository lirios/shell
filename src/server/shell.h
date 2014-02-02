/*
 * Copyright 2013-2014  Giulio Camuffo <giuliocamuffo@gmail.com>
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

#ifndef SHELL_H
#define SHELL_H

#include <list>
#include <vector>
#include <unordered_map>

#include "utils.h"
#include "layer.h"
#include "binding.h"
#include "interface.h"

struct weston_view;

class ShellSurface;
class Effect;
class Workspace;
class ShellSeat;
class Animation;

typedef std::list<ShellSurface *> ShellSurfaceList;

enum class Cursor {
    None = 0,
    ResizeTop = 1,
    ResizeBottom = 2,
    Arrow = 3,
    ResizeLeft = 4,
    ResizeTopLeft = 5,
    ResizeBottomLeft = 6,
    Move = 7,
    ResizeRight = 8,
    ResizeTopRight = 9,
    ResizeBottomRight = 10,
    Busy = 11
};

class Shell;

class ShellGrab {
public:
    ShellGrab();
    virtual ~ShellGrab();

    void start(weston_seat *seat);
    void start(weston_seat *seat, Cursor c);
    void end();

    weston_pointer *pointer() const { return m_pointer; }

    static ShellGrab *fromGrab(weston_pointer_grab *grab);

protected:
    virtual void focus() {}
    virtual void motion(uint32_t time, wl_fixed_t x, wl_fixed_t y);
    virtual void button(uint32_t time, uint32_t button, uint32_t state) {}
    virtual void cancel() {}
    void setCursor(Cursor cursor);
    void unsetCursor();

private:
    weston_pointer *m_pointer;
    struct Grab {
        weston_pointer_grab base;
        ShellGrab *parent;
    } m_grab;

    static const weston_pointer_grab_interface s_shellGrabInterface;

    friend class Shell;
};

class Shell : public Object {
public:
    enum class PanelPosition {
        Top = 0,
        Left = 1,
        Bottom = 2,
        Right = 3
    };

    template<class T>
    static T *load(struct weston_compositor *ec, char *client);
    virtual ~Shell();
    void quit();

    void launchShellProcess();
    virtual ShellSurface *createShellSurface(weston_surface *surface, const weston_shell_client *client);
    void removeShellSurface(ShellSurface *surface);
    static ShellSurface *getShellSurface(const struct weston_surface *surf);
    static weston_view *defaultView(const weston_surface *surface);

    void registerEffect(Effect *effect);

    void configureSurface(ShellSurface *surface, int32_t sx, int32_t sy);

    void setBackgroundSurface(struct weston_surface *surface, struct weston_output *output);
    void setGrabSurface(struct weston_surface *surface);
    void addPanelSurface(weston_surface *surface, weston_output *output, PanelPosition pos);
    void addOverlaySurface(struct weston_surface *surface, struct weston_output *output);

    void showPanels();
    void hidePanels();
    bool isInFullscreen() const;

    virtual IRect2D windowsArea(struct weston_output *output) const;

    struct weston_output *getDefaultOutput() const;
    Workspace *currentWorkspace() const;
    Workspace *workspace(uint32_t id) const;
    void selectPreviousWorkspace();
    void selectNextWorkspace();
    void selectWorkspace(int32_t id);
    uint32_t numWorkspaces() const;

    void showAllWorkspaces();
    void resetWorkspaces();

    void minimizeWindows();
    void restoreWindows();

    struct wl_client *shellClient() { return m_child.client; }
    struct wl_resource *shellClientResource() { return m_child.desktop_shell; }

    static Shell *instance() { return s_instance; }
    inline static weston_compositor *compositor() { return instance()->m_compositor; }

    void bindHotSpot(Binding::HotSpot hs, Binding *b);
    void removeHotSpotBinding(Binding *b);

    void putInLimbo(ShellSurface *s);
    void addStickyView(weston_view *w);

    virtual bool isTrusted(wl_client *client, const char *interface) const;

protected:
    Shell(struct weston_compositor *ec);
    virtual void init();
    inline const ShellSurfaceList &surfaces() const { return m_surfaces; }
    virtual void setGrabCursor(Cursor cursor) {}
    void addWorkspace(Workspace *ws);
    void setSplash(weston_view *view);
    virtual void panelConfigure(struct weston_surface *es, int32_t sx, int32_t sy, PanelPosition pos);

    virtual void defaultPointerGrabFocus(weston_pointer_grab *grab);
    virtual void defaultPointerGrabMotion(weston_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y);
    virtual void defaultPointerGrabButton(weston_pointer_grab *grab, uint32_t time, uint32_t button, uint32_t state);
    virtual void movePointer(weston_pointer *pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y);

    struct Child {
        Shell *shell;
        struct weston_process process;
        struct wl_client *client;
        struct wl_resource *desktop_shell;

        unsigned deathcount;
        uint32_t deathstamp;
    };
    Child m_child;

    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_fullscreenLayer;
    Layer m_stickyLayer;
    Layer m_overlayLayer;

private:
    void destroy(void *);
    void sigchld(int status);
    void backgroundConfigure(struct weston_surface *es, int32_t sx, int32_t sy);
    void activateSurface(struct weston_seat *seat, uint32_t time, uint32_t button);
    void configureFullscreen(ShellSurface *surface);
    void stackFullscreen(ShellSurface *surface);
    weston_view *createBlackSurface(ShellSurface *fs_surface, float x, float y, int w, int h);
    bool surfaceIsTopFullscreen(ShellSurface *surface);
    void activateWorkspace(Workspace *old);
    weston_view *createBlackSurface(int x, int y, int w, int h);
    void workspaceRemoved(Workspace *ws);
    void grabViewDestroyed(void *d);

    struct weston_compositor *m_compositor;
    WlListener m_destroyListener;
    char *m_clientPath;
    Layer m_splashLayer;
    Layer m_limboLayer;
    std::vector<Effect *> m_effects;
    ShellSurfaceList m_surfaces;
    std::vector<Workspace *> m_workspaces;
    uint32_t m_currentWorkspace;
    bool m_windowsMinimized;
    bool m_quitting;
    weston_surface *m_background;

    std::unordered_map<int, std::list<Binding *>> m_hotSpotBindings;
    uint32_t m_lastMotionTime;
    uint32_t m_enterHotZone;

    std::list<weston_view *> m_blackSurfaces;
    weston_view *m_grabView;
    WlListener m_grabViewDestroy;

    static void staticPanelConfigure(weston_surface *es, int32_t sx, int32_t sy);

    static const weston_pointer_grab_interface s_defaultPointerGrabInterface;
    static Shell *s_instance;

    friend class Effect;
    friend ShellGrab;
};

template<class T>
T *Shell::load(struct weston_compositor *ec, char *client)
{
    Shell *shell = new T(ec);
    if (shell) {
        shell->m_clientPath = client;
    }

    return static_cast<T *>(shell);
}

#endif
