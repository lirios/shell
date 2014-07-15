/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013-2014 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#ifndef DESKTOP_SHELL_H
#define DESKTOP_SHELL_H

#include <list>
#include <unordered_map>

#include "shell.h"
#include "utils.h"

class InputPanel;
class WlShell;
class XWlShell;
class HawaiiWorkspace;
class Splash;
class Client;
class Binding;
class SessionManager;
class PanelSurface;

class DesktopShell : public Shell {
public:
    DesktopShell(struct weston_compositor *ec);
    ~DesktopShell();

    weston_output *preferredOutput() const;

    weston_view *createBlackSurfaceWithInput(int x, int y, int w, int h, float a);

    bool isLocked() const { return m_locked; }

    void fadeIn();
    void fadeOut();

    void lockSession();
    void unlockSession();
    void resumeDesktop();

    bool isTrusted(wl_client *client, const char *interface) const override;

    void addPanelSurfaceToLayer(weston_view *view);
    void prependViewToLockLayer(weston_view *view);

    virtual IRect2D windowsArea(struct weston_output *output) const;

    void recalculateAvailableGeometry();

    void centerSurfaceOnOutput(weston_view *ev, weston_output *output);

protected:
    Layer m_desktopLayer;
    Layer m_dialogsLayer;
    Layer m_lockLayer;

    virtual void init();
    virtual void setGrabCursor(Cursor cursor);
    virtual ShellSurface *createShellSurface(weston_surface *surface, const weston_shell_client *client) override;

private:
    void idle(void *);
    void wake(void *);

    void sendInitEvents();

    void workspaceAdded(HawaiiWorkspace *ws);

    void surfaceResponsivenessChanged(ShellSurface *shsurf, bool responsive);

    void bindDesktopShell(struct wl_client *client, uint32_t version, uint32_t id);
    void unbindDesktopShell(struct wl_resource *resource);

    void bindDesktopShellSurface(struct wl_client *client, uint32_t version, uint32_t id);
    void unbindDesktopShellSurface(struct wl_resource *resource);

    void moveBinding(struct weston_seat *seat, uint32_t time, uint32_t button);
    void resizeBinding(struct weston_seat *seat, uint32_t time, uint32_t button);
    void closeBinding(struct weston_seat *seat, uint32_t time, uint32_t button);
    void switcherBinding(weston_seat *seat, uint32_t time, uint32_t button);

    void setBusyCursor(ShellSurface *shsurf, weston_seat *seat);
    void endBusyCursor(weston_seat *seat);

    void trustedClientDestroyed(void *client);

    void addKeyBinding(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t key, uint32_t modifiers);

    void lockSurfaceDestroy(void *);

    void configureViewForAvailableSpace(weston_view *ev, Layer *layer);
    void centerViewOnAvailableSpace(weston_view *ev);

    /*
     * hawaii_shell
     */

    void addTrustedClient(wl_client *client, wl_resource *resource, int32_t fd, const char *interface);

    void setBackground(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                       struct wl_resource *surface_resource);
    void setDesktop(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                    struct wl_resource *surface_resource);

    void setPopup(struct wl_client *client, struct wl_resource *resource,
                  uint32_t id,
                  struct wl_resource *parent_resource,
                  struct wl_resource *surface_resource,
                  int32_t x, int32_t y);

    void setDialog(struct wl_client *client, struct wl_resource *resource,
                   struct wl_resource *output_resource,
                   struct wl_resource *surface_resource);

    void setPosition(struct wl_client *client, struct wl_resource *resource,
                     struct wl_resource *surface_resource,
                     int32_t x, int32_t y);

    void setConfigSurface(wl_client *client, wl_resource *resource,
                          wl_resource *output_resource,
                          wl_resource *surface_resource);

    void setOverlay(wl_client *client, wl_resource *resource,
                    wl_resource *surface_resource);

    void lockSurfaceConfigure(weston_surface *es, int32_t sx, int32_t sy);
    void setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);

    void quit(wl_client *client, wl_resource *resource);

    void lock(wl_client *client, wl_resource *resource);
    void unlock(struct wl_client *client, struct wl_resource *resource);

    void setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);

    void desktopReady(struct wl_client *client, struct wl_resource *resource);

    void addWorkspace(wl_client *client, wl_resource *resource);
    void selectWorkspace(wl_client *client, wl_resource *resource, wl_resource *workspace_resource);

    void createGrab(wl_client *client, wl_resource *resource, uint32_t id);

    static void configurePopup(weston_surface *es, int32_t sx, int32_t sy);

    static const struct hawaii_shell_interface m_desktopShellImpl;
    static const struct hawaii_shell_surface_interface m_shellSurfaceImpl;

    WlListener m_idleListener;
    WlListener m_wakeListener;

    struct Output {
        weston_output *output;
        wl_resource *resource;
        IRect2D rect;
    };
    std::list<Output> m_outputs;
    InputPanel *m_inputPanel;
    std::unordered_map<std::string, std::list<Client *>> m_trustedClients;
    Splash *m_splash;
    Binding *m_moveBinding;
    Binding *m_resizeBinding;
    Binding *m_closeBinding;
    Binding *m_prevWsBinding;
    Binding *m_nextWsBinding;
    Binding *m_quitBinding;
    SessionManager *m_sessionManager;

    std::list<PanelSurface *> m_panels;

    std::list<wl_resource *> m_shellSurfaceBindings;

    bool m_prepareEventSent;
    bool m_locked;
    weston_view *m_lockSurface;
    WlListener m_lockSurfaceDestroyListener;

    friend class DesktopShellSettings;
    friend int module_init(weston_compositor *ec, int *argc, char *argv[]);
};

#endif
