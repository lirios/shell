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

#ifndef SHELL_H
#define SHELL_H

#include <vector>

#include "utils.h"
#include "layer.h"

class ShellSurface;
struct ShellGrab;
class Effect;
class Workspace;
class ShellSeat;

typedef std::vector<ShellSurface *> ShellSurfaceList;

class Shell;

struct ShellGrab {
    ShellGrab();

    Shell *shell;
    struct wl_pointer_grab grab;
    struct wl_pointer *pointer;
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
    static Shell *load(struct weston_compositor *ec);
    ~Shell();

    void launchShellProcess();
    ShellSurface *createShellSurface(struct weston_surface *surface, const struct weston_shell_client *client);
    ShellSurface *getShellSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id, struct wl_resource *surface_resource);
    void removeShellSurface(ShellSurface *surface);
    static ShellSurface *getShellSurface(struct weston_surface *surf);
    Binding *bindKey(uint32_t key, enum weston_keyboard_modifier modifier, weston_key_binding_handler_t handler, void *data);
    template<class T>
    Binding *bindKey(uint32_t key, enum weston_keyboard_modifier modifier,
                                   void (T::*func)(struct wl_seat *seat, uint32_t time, uint32_t key), T *obj);

    Binding *bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier, weston_axis_binding_handler_t handler, void *data);
    template<class T>
    Binding *bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier,
                      void (T::*func)(struct wl_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value), T *obj);

    void registerEffect(Effect *effect);

    void configureSurface(ShellSurface *surface, int32_t sx, int32_t sy, int32_t width, int32_t height);

    void activateSurface(ShellSurface *shsurf, struct weston_seat *seat);
    void setBackgroundSurface(struct weston_surface *surface, struct weston_output *output);
    void setGrabSurface(struct weston_surface *surface);
    void addPanelSurface(struct weston_surface *surface, struct weston_output *output);

    void startGrab(ShellGrab *grab, const struct wl_pointer_grab_interface *interface,
                   struct weston_seat *seat, uint32_t cursor);
    static void endGrab(ShellGrab *grab);

    void showPanels();
    void hidePanels();
    bool isInFullscreen() const;

    virtual IRect2D windowsArea(struct weston_output *output) const;

    inline struct weston_compositor *compositor() const { return m_compositor; }
    struct weston_output *getDefaultOutput() const;
    Workspace *currentWorkspace() const;
    Workspace *workspace(uint32_t id) const;
    void selectPreviousWorkspace();
    void selectNextWorkspace();
    void selectWorkspace(uint32_t id);
    uint32_t numWorkspaces() const;

    void showAllWorkspaces();
    void resetWorkspaces();

protected:
    Shell(struct weston_compositor *ec);
    virtual void init();
    inline const ShellSurfaceList &surfaces() const { return m_surfaces; }
    virtual void setGrabCursor(uint32_t cursor) {}
    virtual void setBusyCursor(ShellSurface *shsurf, struct weston_seat *seat) {}
    virtual void endBusyCursor(struct weston_seat *seat) {}

    struct Child {
        Shell *shell;
        struct weston_process process;
        struct wl_client *client;
        struct wl_resource *desktop_shell;

        unsigned deathcount;
        uint32_t deathstamp;
    };
    Child m_child;

private:
    void bind(struct wl_client *client, uint32_t version, uint32_t id);
    void sigchld(int status);
    void backgroundConfigure(struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height);
    void panelConfigure(struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height);
    void activateSurface(struct wl_seat *seat, uint32_t time, uint32_t button);
    void configureFullscreen(ShellSurface *surface);
    void stackFullscreen(ShellSurface *surface);
    struct weston_surface *createBlackSurface(ShellSurface *fs_surface, float x, float y, int w, int h);
    static void sendConfigure(struct weston_surface *surface, uint32_t edges, int32_t width, int32_t height);
    bool surfaceIsTopFullscreen(ShellSurface *surface);
    void activateWorkspace(Workspace *old);
    void pointerFocus(ShellSeat *shseat, struct wl_pointer *pointer);
    void pingTimeout(ShellSurface *shsurf);
    void pong(ShellSurface *shsurf);

    struct weston_compositor *m_compositor;
    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_fullscreenLayer;
    std::vector<Effect *> m_effects;
    ShellSurfaceList m_surfaces;
    std::vector<Workspace *> m_workspaces;
    uint32_t m_currentWorkspace;

    struct weston_surface *m_blackSurface;
    struct weston_surface *m_grabSurface;

    static const struct wl_shell_interface shell_implementation;
    static const struct weston_shell_client shell_client;

    friend class Effect;
};

template<class T>
Shell *Shell::load(struct weston_compositor *ec)
{
    Shell *shell = new T(ec);
    if (shell) {
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
                        void (T::*func)(struct wl_seat *seat, uint32_t time, uint32_t key), T *obj) {
    MemberBinding<T, struct wl_seat *, uint32_t, uint32_t> *binding = new MemberBinding<T, struct wl_seat *, uint32_t, uint32_t>(obj, func);
    binding->m_binding = weston_compositor_add_key_binding(m_compositor, key, modifier,
                                                           MemberBinding<T, struct wl_seat *, uint32_t, uint32_t>::handler, binding);
    return binding;
}

template<class T>
Binding *Shell::bindAxis(uint32_t axis, enum weston_keyboard_modifier modifier,
                         void (T::*func)(struct wl_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value), T *obj) {
    MemberBinding<T, struct wl_seat *, uint32_t, uint32_t, wl_fixed_t> *binding =
        new MemberBinding<T, struct wl_seat *, uint32_t, uint32_t, wl_fixed_t>(obj, func);
    binding->m_binding = weston_compositor_add_axis_binding(m_compositor, axis, modifier,
                         MemberBinding<T, struct wl_seat *, uint32_t, uint32_t, wl_fixed_t>::handler, binding);
    return binding;
}

#endif
