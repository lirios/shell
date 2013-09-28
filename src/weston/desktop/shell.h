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

#ifndef SHELL_H
#define SHELL_H

#include <list>
#include <vector>
#include <map>

#include "utils.h"
#include "layer.h"

class ShellSurface;
struct ShellGrab;
class Effect;
class Workspace;
class ShellSeat;
class Animation;

typedef std::list<ShellSurface *> ShellSurfaceList;

class Shell;

struct ShellGrab {
    ShellGrab();

    Shell *shell;
    struct weston_pointer_grab grab;
    struct weston_pointer *pointer;
};

class Binding {
public:
    virtual ~Binding();

protected:
    explicit Binding(struct weston_binding *binding);

private:
    struct weston_binding *m_binding;

    friend class Shell;
};

class Shell {
public:
    template<class T>
    static Shell *load(struct weston_compositor *ec, char *client);
    virtual ~Shell();

    void launchShellProcess();
    ShellSurface *createShellSurface(struct weston_surface *surface, const struct weston_shell_client *client);
    ShellSurface *getShellSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface_resource);
    void removeShellSurface(ShellSurface *surface);
    static ShellSurface *getShellSurface(const struct weston_surface *surf);
    Binding *bindKey(uint32_t key, enum weston_keyboard_modifier modifier, weston_key_binding_handler_t handler, void *data);
    template<class T>
    Binding *bindKey(uint32_t key, enum weston_keyboard_modifier modifier,
                                   void (T::*func)(struct weston_seat *seat, uint32_t time, uint32_t key), T *obj);

    Binding *bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier, weston_axis_binding_handler_t handler, void *data);
    template<class T>
    Binding *bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier,
                      void (T::*func)(struct weston_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value), T *obj);

    virtual void lockSession();
    virtual void unlockSession();

    void quit();

    void registerEffect(Effect *effect);

    void centerSurfaceOnOutput(struct weston_surface *es, struct weston_output *output);

    void configureSurface(ShellSurface *surface, int32_t sx, int32_t sy, int32_t width, int32_t height);

    void setGrabSurface(struct weston_surface *surface);

    void startGrab(ShellGrab *grab, const struct weston_pointer_grab_interface *interface,
                   struct weston_seat *seat, uint32_t cursor);
    static void endGrab(ShellGrab *grab);

    void showPanels();
    void hidePanels();

    void showNotifications();
    void hideNotifications();

    void showOverlays();
    void hideOverlays();

    bool isInFullscreen() const;

    virtual IRect2D windowsArea(struct weston_output *output);

    inline struct weston_compositor *compositor() const { return m_compositor; }
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

    weston_surface *createBlackSurface(int x, int y, int w, int h);
    weston_surface *createBlackSurface(ShellSurface *fs_surface, float x, float y, int w, int h);
    bool isBlackSurface(weston_surface *es, weston_surface **fs_surface);

protected:
    Shell(struct weston_compositor *ec);

    virtual void init();

    inline const ShellSurfaceList &surfaces() const { return m_surfaces; }
    virtual void setGrabCursor(uint32_t cursor) {}
    virtual void setBusyCursor(ShellSurface *shsurf, struct weston_seat *seat) {}
    virtual void endBusyCursor(struct weston_seat *seat) {}
    void fadeIn();
    void fadeOut();
    void addWorkspace(Workspace *ws);

    struct Child {
        Shell *shell;
        struct weston_process process;
        struct wl_client *client;
        struct wl_resource *desktop_shell;

        unsigned deathcount;
        uint32_t deathstamp;
    };
    Child m_child;

    struct weston_compositor *m_compositor;
    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_fullscreenLayer;
    Layer m_overlayLayer;
    Layer m_notificationsLayer;
    Layer m_lockLayer;

    std::map<struct weston_output *, IRect2D> m_windowsArea;

private:
    void destroy();

    void idle();
    void wake();

    void bind(struct wl_client *client, uint32_t version, uint32_t id);
    void sigchld(int status);

    void activateSurface(struct weston_seat *seat, uint32_t time, uint32_t button);
    void configureFullscreen(ShellSurface *surface);
    void stackFullscreen(ShellSurface *surface);
    static void sendConfigure(struct weston_surface *surface, uint32_t edges, int32_t width, int32_t height);
    bool surfaceIsTopFullscreen(ShellSurface *surface);
    void activateWorkspace(Workspace *old);
    void pointerFocus(ShellSeat *shseat, struct weston_pointer *pointer);
    void pingTimeout(ShellSurface *shsurf);
    void pong(ShellSurface *shsurf);
    void workspaceRemoved(Workspace *ws);

    WlListener m_destroyListener;
    WlListener m_idleListener;
    WlListener m_wakeListener;
    char *m_clientPath;
    std::vector<Effect *> m_effects;
    ShellSurfaceList m_surfaces;
    std::vector<Workspace *> m_workspaces;
    uint32_t m_currentWorkspace;
    bool m_windowsMinimized;

    class Splash *m_splash;
    struct weston_surface *m_grabSurface;

    static const struct wl_shell_interface shell_implementation;
    static const struct weston_shell_client shell_client;

    friend class Effect;
};

template<class T>
Shell *Shell::load(struct weston_compositor *ec, char *client)
{
    Shell *shell = new T(ec);
    if (shell) {
        shell->m_clientPath = client;
        shell->init();
    }

    return shell;
}

template<class T, class... Args>
class MemberBinding : public Binding {
public:

private:
    typedef void (T::*Func)(Args...);

    MemberBinding(T *obj, Func func) : Binding(nullptr), m_obj(obj), m_func(func) {}
    static void handler(Args... args, void *data) {
        MemberBinding<T, Args...> *bind = static_cast<MemberBinding<T, Args...> *>(data);
        (bind->m_obj->*bind->m_func)(args...);
    }

    T *m_obj;
    Func m_func;

    friend class Shell;
};

template<class T>
Binding *Shell::bindKey(uint32_t key, enum weston_keyboard_modifier modifier,
                        void (T::*func)(struct weston_seat *seat, uint32_t time, uint32_t key), T *obj) {
    MemberBinding<T, struct weston_seat *, uint32_t, uint32_t> *binding = new MemberBinding<T, struct weston_seat *, uint32_t, uint32_t>(obj, func);
    binding->m_binding = weston_compositor_add_key_binding(m_compositor, key, modifier,
                                                           MemberBinding<T, struct weston_seat *, uint32_t, uint32_t>::handler, binding);
    return binding;
}

template<class T>
Binding *Shell::bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier,
                         void (T::*func)(struct weston_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value), T *obj) {
    MemberBinding<T, struct weston_seat *, uint32_t, uint32_t, wl_fixed_t> *binding =
    new MemberBinding<T, struct weston_seat *, uint32_t, uint32_t, wl_fixed_t>(obj, func);
    binding->m_binding = weston_compositor_add_axis_binding(m_compositor, axis, modifier,
                         MemberBinding<T, struct weston_seat *, uint32_t, uint32_t, wl_fixed_t>::handler, binding);
    return binding;
}

#endif
