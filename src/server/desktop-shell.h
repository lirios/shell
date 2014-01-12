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

#include "shell.h"

class InputPanel;

class DesktopShell : public Shell {
public:
    enum Edge {
        EdgeTop = 1 << 0,
        EdgeBottom = 1 << 1,
        EdgeLeft = 1 << 2,
        EdgeRight = 1 << 3
    };

    enum CornerAnchors {
        CornerTopRight = EdgeTop | EdgeRight,
        CornerTopLeft = EdgeTop | EdgeLeft,
        CornerBottomRight = EdgeBottom | EdgeRight,
        CornerBottomLeft = EdgeBottom | EdgeLeft
    };

    enum NotificationsOrder {
        OrderNewestFirst = 0,
        OrderOldestFirst
    };

    DesktopShell(struct weston_compositor *ec);

protected:
    virtual void init();
    virtual void setGrabCursor(uint32_t);
    virtual void setBusyCursor(ShellSurface *shsurf, struct weston_seat *seat) override;
    virtual void endBusyCursor(struct weston_seat *seat) override;

private:
    void sendInitEvents();
    void workspaceAdded(Workspace *ws);

    void bindNotifications(wl_client *client, uint32_t version, uint32_t id);
    void unbindNotifications(wl_resource *resource);

    void bindDesktopShell(struct wl_client *client, uint32_t version, uint32_t id);
    void unbindDesktopShell(struct wl_resource *resource);

    void bindDesktopShellSurface(struct wl_client *client, uint32_t version, uint32_t id);
    void unbindDesktopShellSurface(struct wl_resource *resource);

    void bindScreenSaver(wl_client *client, uint32_t version, uint32_t id);
    void unbindScreenSaver(wl_resource *resource);

    void moveBinding(struct weston_seat *seat, uint32_t time, uint32_t button);
    void resizeBinding(weston_seat *seat, uint32_t time, uint32_t button);

    void switcherBinding(weston_seat *seat, uint32_t time, uint32_t button);

    void addKeyBinding(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t key, uint32_t modifiers);

    void lockSession();
    void unlockSession();

    void resumeDesktop();

    void lockSurfaceDestroy();

    /*
     * wl_hawaii_shell
     */

    void setAvailableGeometry(struct wl_client *client, struct wl_resource *resource,
                              struct wl_resource *output_resource,
                              int32_t x, int32_t y,
                              int32_t width, int32_t height);

    void setBackground(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                       struct wl_resource *surface_resource);

    void setPanel(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                  struct wl_resource *surface_resource);

    void setLauncher(struct wl_client *client, struct wl_resource *resource,
                     struct wl_resource *output_resource,
                     struct wl_resource *surface_resource);

    void setOverlay(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource, struct wl_resource *surface_resource);

    void setPopup(struct wl_client *client, struct wl_resource *resource,
                  uint32_t id,
                  struct wl_resource *output_resource,
                  struct wl_resource *parent_resource,
                  struct wl_resource *surface_resource,
                  int32_t x, int32_t y);

    void setDialog(struct wl_client *client, struct wl_resource *resource,
                   struct wl_resource *output_resource,
                   struct wl_resource *surface_resource);

    void setPosition(struct wl_client *client, struct wl_resource *resource,
                     struct wl_resource *surface_resource,
                     int32_t x, int32_t y);

    void lockSurfaceConfigure(weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height);
    void setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);

    void quit(wl_client *client, wl_resource *resource);

    void lock(wl_client *client, wl_resource *resource);
    void unlock(struct wl_client *client, struct wl_resource *resource);

    void setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource);

    void desktopReady(struct wl_client *client, struct wl_resource *resource);

    void addWorkspace(wl_client *client, wl_resource *resource);
    void selectWorkspace(wl_client *client, wl_resource *resource, wl_resource *workspace_resource);

    void createGrab(wl_client *client, wl_resource *resource, uint32_t id);

    /*
     * wl_notification_daemon
     */

    void notificationConfigure(weston_surface *es, int32_t sx, int32_t sy,
                               int32_t width, int32_t height);
    void mapNotificationSurfaces();
    void addNotificationSurface(wl_client *client, wl_resource *resource,
                                wl_resource *surface_resource);

    /*
     * screensaver
     */

    void screenSaverSigChild(int status);

    void launchScreenSaverProcess();
    void terminateScreenSaverProcess();

    int screenSaverTimeout();

    void screenSaverConfigure(weston_surface *es, int32_t sx, int32_t sy,
                              int32_t width, int32_t height);
    void setScreenSaverSurface(wl_client *client, wl_resource *resource,
                               wl_resource *output_resource,
                               wl_resource *surface_resource);

    static const struct wl_hawaii_shell_interface m_desktopShellImpl;
    static const struct wl_hawaii_shell_surface_interface m_shellSurfaceImpl;
    static const struct wl_notification_daemon_interface m_notificationDaemonImpl;
    static const struct wl_screensaver_interface m_screenSaverImpl;

    std::list<wl_resource *> m_shellSurfaceBindings;
    wl_resource *m_screenSaverBinding;
    wl_event_source *m_screenSaverTimer;
    std::string m_screenSaverPath;
    int m_screenSaverDuration;

    struct ScreenSaverChild {
        DesktopShell *shell;
        struct weston_process process;
        struct wl_client *client;
    };
    ScreenSaverChild m_screenSaverChild;

    InputPanel *m_inputPanel;

    Edge m_notificationsEdge;
    CornerAnchors m_notificationsCornerAnchor;
    NotificationsOrder m_notificationsOrder;
    int32_t m_notificationsMargin;

    bool m_prepareEventSent;
    bool m_locked;
    struct weston_surface *m_lockSurface;
    WlListener m_lockSurfaceDestroyListener;
};

#endif
