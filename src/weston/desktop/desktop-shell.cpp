/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/input.h>

#include <wayland-server.h>

#include <weston/compositor.h>
#include <weston/matrix.h>
// #include <weston/config-parser.h>

#include "desktop-shell.h"
#include "wayland-desktop-shell-server-protocol.h"
#include "wayland-notification-daemon-server-protocol.h"
#include "wayland-screensaver-server-protocol.h"
#include "shellsurface.h"

#include "scaleeffect.h"
#include "griddesktops.h"
#include "fademovingeffect.h"
#include "zoomeffect.h"
#include "inoutsurfaceeffect.h"
#include "inputpanel.h"
#include "shellseat.h"
#include "workspace.h"
#include "minimizeeffect.h"

DesktopShell::DesktopShell(struct weston_compositor *ec)
            : Shell(ec)
            , m_screenSaverBinding(nullptr)
            , m_screenSaverDuration(5*60*1000)
            , m_inputPanel(nullptr)
            , m_notificationsEdge(DesktopShell::EdgeRight)
            , m_notificationsCornerAnchor(DesktopShell::CornerTopRight)
            , m_notificationsOrder(DesktopShell::OrderNewestFirst)
            , m_notificationsMargin(20)
            , m_prepareEventSent(false)
            , m_locked(false)
            , m_lockSurface(nullptr)
{
    m_screenSaverChild.shell = this;
    m_screenSaverChild.process.pid = 0;
}

void DesktopShell::init()
{
    Shell::init();

    if (!wl_global_create(compositor()->wl_display, &wl_notification_daemon_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindNotifications(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &hawaii_desktop_shell_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindDesktopShell(client, version, id); }))
        return;

    if (!wl_global_create(compositor()->wl_display, &screensaver_interface, 1, this,
                          [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bindScreenSaver(client, version, id); }))
        return;

    struct wl_event_loop *loop = wl_display_get_event_loop(m_compositor->wl_display);
    m_screenSaverTimer = wl_event_loop_add_timer(loop, [](void *data) {
        return static_cast<DesktopShell *>(data)->screenSaverTimeout(); }, this);

    weston_compositor_add_button_binding(compositor(), BTN_LEFT, MODIFIER_SUPER,
                                         [](struct weston_seat *seat, uint32_t time, uint32_t button, void *data) {
        static_cast<DesktopShell *>(data)->moveBinding(seat, time, button);
    }, this);

    new ScaleEffect(this);
    new GridDesktops(this);
    new FadeMovingEffect(this);
    new ZoomEffect(this);
    new InOutSurfaceEffect(this);
    new MinimizeEffect(this);

    m_inputPanel = new InputPanel(compositor()->wl_display);
}

void DesktopShell::setGrabCursor(uint32_t cursor)
{
    hawaii_desktop_shell_send_grab_cursor(m_child.desktop_shell, cursor);
}

struct BusyGrab : public ShellGrab {
    ShellSurface *surface;
};

static void busy_cursor_grab_focus(struct weston_pointer_grab *base)
{
    BusyGrab *grab = static_cast<BusyGrab *>(container_of(base, ShellGrab, grab));
    wl_fixed_t sx, sy;
    struct weston_surface *surface = weston_compositor_pick_surface(grab->pointer->seat->compositor,
                                                                    grab->pointer->x, grab->pointer->y,
                                                                    &sx, &sy);

    if (grab->surface->weston_surface() != surface) {
        Shell::endGrab(grab);
        delete grab;
    }
}

static void busy_cursor_grab_button(struct weston_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state)
{
    BusyGrab *grab = static_cast<BusyGrab *>(container_of(base, ShellGrab, grab));

    struct weston_seat *seat = grab->pointer->seat;

    if (grab->surface && button == BTN_LEFT && state) {
        ShellSeat::shellSeat(seat)->activate(grab->surface);
        grab->surface->dragMove(seat);
    } else if (grab->surface && button == BTN_RIGHT && state) {
        ShellSeat::shellSeat(seat)->activate(grab->surface);
        //         surface_rotate(grab->surface, &seat->seat);
    }
}

static const struct weston_pointer_grab_interface busy_cursor_grab_interface = {
    busy_cursor_grab_focus,
    [](struct weston_pointer_grab *grab, uint32_t time) {},
    busy_cursor_grab_button,
};

void DesktopShell::setBusyCursor(ShellSurface *surface, struct weston_seat *seat)
{
    BusyGrab *grab = new BusyGrab;
    if (!grab && grab->pointer)
        return;

    grab->pointer = seat->pointer;
    grab->surface = surface;
    startGrab(grab, &busy_cursor_grab_interface, seat, HAWAII_DESKTOP_SHELL_CURSOR_BUSY);
}

void DesktopShell::endBusyCursor(struct weston_seat *seat)
{
    ShellGrab *grab = container_of(seat->pointer->grab, ShellGrab, grab);

    if (grab->grab.interface == &busy_cursor_grab_interface) {
        endGrab(grab);
        delete grab;
    }
}

void DesktopShell::sendInitEvents()
{
    for (uint i = 0; i < numWorkspaces(); ++i) {
        workspace(i)->init(m_child.client);
        workspaceAdded(workspace(i));
    }

    for (ShellSurface *shsurf: surfaces()) {
        shsurf->advertize();
    }
}

void DesktopShell::workspaceAdded(Workspace *ws)
{
    hawaii_desktop_shell_send_workspace_added(m_child.desktop_shell, ws->resource(), ws->active());
}

void DesktopShell::bindNotifications(wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &wl_notification_daemon_interface, version, id);

    if (resource) {
        wl_resource_set_implementation(resource, &m_notificationDaemonImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindNotifications(resource); });
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_notification_daemon denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindNotifications(struct wl_resource *resource)
{
    free(resource);
}

void DesktopShell::bindDesktopShell(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &hawaii_desktop_shell_interface, version, id);

    if (client == m_child.client) {
        wl_resource_set_implementation(resource, &m_desktopShellImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindDesktopShell(resource); });
        m_child.desktop_shell = resource;

        sendInitEvents();
        hawaii_desktop_shell_send_loaded(resource);
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind hawaii_desktop_shell denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindDesktopShell(struct wl_resource *resource)
{
    if (m_locked)
        resumeDesktop();

    m_child.desktop_shell = nullptr;
    m_prepareEventSent = false;
}

void DesktopShell::bindScreenSaver(wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &screensaver_interface, version, id);

    if (!m_screenSaverBinding) {
        wl_resource_set_implementation(resource, &m_screenSaverImpl, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbindScreenSaver(resource); });
        m_screenSaverBinding = resource;
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind wl_notification_daemon denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbindScreenSaver(struct wl_resource *resource)
{
    m_screenSaverBinding = nullptr;
}

void DesktopShell::moveBinding(struct weston_seat *seat, uint32_t time, uint32_t button)
{
    struct weston_surface *surface = seat->pointer->focus;
    if (!surface) {
        return;
    }

    ShellSurface *shsurf = getShellSurface(surface);
    if (!shsurf || shsurf->type() == ShellSurface::Type::Fullscreen || shsurf->type() == ShellSurface::Type::Maximized) {
        return;
    }

    shsurf = shsurf->topLevelParent();
    if (shsurf) {
        shsurf->dragMove(seat);
    }
}

void DesktopShell::addKeyBinding(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t key, uint32_t modifiers)
{
    wl_resource *res = wl_resource_create(client, &hawaii_key_binding_interface, wl_resource_get_version(resource), id);
    wl_resource_set_implementation(res, nullptr, res, [](wl_resource *) {});

    weston_compositor_add_key_binding(compositor(), key, (weston_keyboard_modifier)modifiers,
                                      [](struct weston_seat *seat, uint32_t time, uint32_t key, void *data) {

        hawaii_key_binding_send_triggered(static_cast<wl_resource *>(data));
    }, res);
}

void DesktopShell::lockSession()
{
    if (m_locked) {
        weston_compositor_sleep(m_compositor);
        return;
    }

    m_locked = true;

    // Hide all surfaces by removing all layers except for backgrounds,
    // this way nothing else can show or receive input events while
    // we are locked.
    // We don't remove the backgrounds layer because currently we only
    // show the lock screen on the default output so we need to show
    // something on the other outputs as well as the default one.
    // However this is safe because our background only shows a wallpaper
    // so it won't do much with input events, in the future we need to
    // show the lock screen on all outpus and remove the backgrounds
    // layer as well.
    // TODO: Proper multiscreen setup
    m_lockLayer.show();
    hideOverlays();
    hideNotifications();
    m_fullscreenLayer.hide();
    hidePanels();
    currentWorkspace()->hide();

    launchScreenSaverProcess();

    // TODO: Disable bindings that are not supposed to work while locked

    // All this must be undone in resumeDesktop()
}

void DesktopShell::unlockSession()
{
    weston_log("unlocking session...\n");

    if (!m_locked || m_lockSurface) {
        fadeIn();
        return;
    }

    // If the client has gone away, unlock immediately
    if (!shellClientResource()) {
        resumeDesktop();
        return;
    }

    if (m_prepareEventSent)
        return;

    hawaii_desktop_shell_send_prepare_lock_surface(shellClientResource());
    m_prepareEventSent = true;
}

void DesktopShell::resumeDesktop()
{
    terminateScreenSaverProcess();

    showOverlays();
    showNotifications();
    m_fullscreenLayer.show();
    showPanels();
    currentWorkspace()->show();
    m_lockLayer.hide();

    m_locked = false;
    fadeIn();
    weston_compositor_damage_all(m_compositor);
}

void DesktopShell::lockSurfaceDestroy()
{
    weston_log("lock surface gone\n");
    m_lockSurface = nullptr;
}

void DesktopShell::setAvailableGeometry(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *output_resource,
                                        int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct weston_output *output = static_cast<weston_output *>(output_resource->data);

    IRect2D area(x, y, width, height);
    m_windowsArea[output] = area;
}

void DesktopShell::setBackground(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                                 struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    setBackgroundSurface(surface, static_cast<weston_output *>(output_resource->data));
}

void DesktopShell::setPanel(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                            struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    addPanelSurface(surface, static_cast<weston_output *>(output_resource->data));
}

void DesktopShell::setLauncher(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource,
                               struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    addPanelSurface(surface, static_cast<weston_output *>(output_resource->data));
}

void DesktopShell::setSpecial(struct wl_client *client, struct wl_resource *resource,
                              struct wl_resource *output_resource,
                              struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    addPanelSurface(surface, static_cast<weston_output *>(output_resource->data));
}

void DesktopShell::setOverlay(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource, struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    addOverlaySurface(surface, static_cast<weston_output *>(output_resource->data));
    pixman_region32_fini(&surface->pending.input);
    pixman_region32_init_rect(&surface->pending.input, 0, 0, 0, 0);
}

void DesktopShell::setPosition(struct wl_client *client, struct wl_resource *resource,
                               struct wl_resource *surface_resource,
                               int32_t x, int32_t y)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (!surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role not yet assigned");
        return;
    }

    // Make sure the surface is in the panels layer
    bool found = false;
    for (weston_surface *current: m_panelsLayer) {
        if (current == surface) {
            found = true;
            break;
        }
    }
    if (!found)
        m_panelsLayer.addSurface(surface);

    // Set given position
    weston_surface_set_position(surface, x, y);
}

void DesktopShell::lockSurfaceConfigure(weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    if (width == 0)
        return;

    es->geometry.width = width;
    es->geometry.height = height;

    if (!weston_surface_is_mapped(es)) {
        m_lockLayer.addSurface(es);
        weston_surface_update_transform(es);
        fadeIn();
    }
}

void DesktopShell::setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    m_prepareEventSent = false;

    if (!m_locked)
        return;

    m_lockSurface = surface;

    m_lockSurfaceDestroyListener.listen(&surface->destroy_signal);
    m_lockSurfaceDestroyListener.signal->connect(this, &DesktopShell::lockSurfaceDestroy);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<DesktopShell *>(es->configure_private)->lockSurfaceConfigure(es, sx, sy, width, height); };
    surface->configure_private = this;
}

void DesktopShell::quit(wl_client *client, wl_resource *resource)
{
    Shell::quit();
}

void DesktopShell::lock(wl_client *client, wl_resource *resource)
{
    // Don't lock again if we are already locked
    if (m_locked)
        return;

    fadeOut();
    weston_compositor_damage_all(m_compositor);
}

void DesktopShell::unlock(struct wl_client *client, struct wl_resource *resource)
{
    m_prepareEventSent = false;

    if (m_locked)
        resumeDesktop();
}

void DesktopShell::setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
    this->Shell::setGrabSurface(static_cast<struct weston_surface *>(surface_resource->data));
}

void DesktopShell::desktopReady(struct wl_client *client, struct wl_resource *resource)
{
    fadeIn();
}

void DesktopShell::minimizeWindows(wl_client *client, wl_resource *resource)
{
    Shell::minimizeWindows();
}

void DesktopShell::restoreWindows(wl_client *client, wl_resource *resource)
{
    Shell::restoreWindows();
}

void DesktopShell::addWorkspace(wl_client *client, wl_resource *resource)
{
    Workspace *ws = new Workspace(this, numWorkspaces());
    ws->init(client);
    Shell::addWorkspace(ws);
    workspaceAdded(ws);
}

void DesktopShell::selectWorkspace(wl_client *client, wl_resource *resource, wl_resource *workspace_resource)
{
    Shell::selectWorkspace(Workspace::fromResource(workspace_resource)->number());
}

class ClientGrab : public ShellGrab {
public:
    wl_resource *resource;
    weston_surface *focus;
};

static void client_grab_focus(struct weston_pointer_grab *base)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);
    ClientGrab *cgrab = static_cast<ClientGrab *>(grab);

    wl_fixed_t sx, sy;
    struct weston_surface *surface = weston_compositor_pick_surface(grab->pointer->seat->compositor,
                                                                    grab->pointer->x, grab->pointer->y,
                                                                    &sx, &sy);
    if (cgrab->focus != surface) {
        cgrab->focus = surface;
        hawaii_grab_send_focus(cgrab->resource, surface->resource, sx, sy);
    }
}

static void client_grab_motion(struct weston_pointer_grab *base, uint32_t time)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);
    ClientGrab *cgrab = static_cast<ClientGrab *>(grab);

    wl_fixed_t sx = cgrab->pointer->x;
    wl_fixed_t sy = cgrab->pointer->y;
    if (cgrab->focus) {
        weston_surface_from_global_fixed(cgrab->focus, cgrab->pointer->x, cgrab->pointer->y, &sx, &sy);
    }

    hawaii_grab_send_motion(cgrab->resource, time, sx, sy);
}

static void client_grab_button(struct weston_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);
    ClientGrab *cgrab = static_cast<ClientGrab *>(grab);

    if (grab->pointer->focus_resource) {
        wl_display *display = wl_client_get_display(wl_resource_get_client(grab->pointer->focus_resource));
        uint32_t serial = wl_display_next_serial(display);
        wl_pointer_send_button(grab->pointer->focus_resource, serial, time, button, state);
    }

    hawaii_grab_send_button(cgrab->resource, time, button, state);
}

static const struct weston_pointer_grab_interface client_grab_interface = {
    client_grab_focus,
    client_grab_motion,
    client_grab_button,
};

void client_grab_end(wl_client *client, wl_resource *resource)
{
    ClientGrab *cg = static_cast<ClientGrab *>(resource->data);
    Shell::endGrab(cg);
    weston_output_schedule_repaint(cg->pointer->focus->output);
}

static const struct hawaii_grab_interface hawaii_grab_implementation = {
    client_grab_end
};

void DesktopShell::createGrab(wl_client *client, wl_resource *resource, uint32_t id)
{
    wl_resource *res = wl_resource_create(client, &hawaii_grab_interface, wl_resource_get_version(resource), id);

    ClientGrab *grab = new ClientGrab;
    wl_resource_set_implementation(res, &hawaii_grab_implementation, grab, [](wl_resource *) {});

    if (!grab)
        return;

    weston_seat *seat = container_of(compositor()->seat_list.next, weston_seat, link);
    grab->pointer = seat->pointer;
    grab->resource = res;
    grab->shell = this;

    ShellSeat::shellSeat(seat)->endPopupGrab();

    wl_fixed_t sx, sy;
    struct weston_surface *surface = weston_compositor_pick_surface(compositor(),
                                                                    grab->pointer->x, grab->pointer->y,
                                                                    &sx, &sy);
    weston_pointer_set_focus(grab->pointer, surface, sx, sy);
    grab->focus = surface;
    hawaii_grab_send_focus(grab->resource, surface->resource, sx, sy);

    grab->grab.interface = &client_grab_interface;
    weston_pointer_start_grab(grab->pointer, &grab->grab);
}

const struct hawaii_desktop_shell_interface DesktopShell::m_desktopShellImpl = {
    wrapInterface(&DesktopShell::addKeyBinding),
    wrapInterface(&DesktopShell::setAvailableGeometry),
    wrapInterface(&DesktopShell::setBackground),
    wrapInterface(&DesktopShell::setPanel),
    wrapInterface(&DesktopShell::setLauncher),
    wrapInterface(&DesktopShell::setSpecial),
    wrapInterface(&DesktopShell::setOverlay),
    wrapInterface(&DesktopShell::setPosition),
    wrapInterface(&DesktopShell::setLockSurface),
    wrapInterface(&DesktopShell::quit),
    wrapInterface(&DesktopShell::lock),
    wrapInterface(&DesktopShell::unlock),
    wrapInterface(&DesktopShell::setGrabSurface),
    wrapInterface(&DesktopShell::desktopReady),
    wrapInterface(&DesktopShell::minimizeWindows),
    wrapInterface(&DesktopShell::restoreWindows),
    wrapInterface(&DesktopShell::addWorkspace),
    wrapInterface(&DesktopShell::selectWorkspace),
    wrapInterface(&DesktopShell::createGrab)
};

void DesktopShell::notificationConfigure(weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    if (width == 0)
        return;

    es->geometry.width = weston_surface_buffer_width(es);
    es->geometry.height = weston_surface_buffer_height(es);

    mapNotificationSurfaces();

    weston_compositor_schedule_repaint(m_compositor);
}

void DesktopShell::mapNotificationSurfaces()
{
    bool isRight = m_notificationsCornerAnchor & DesktopShell::EdgeRight;
    bool isBottom = m_notificationsCornerAnchor & DesktopShell::EdgeBottom;

    IRect2D availableGeometry = windowsArea(getDefaultOutput());
    int32_t left = availableGeometry.x;
    int32_t top = availableGeometry.y;
    int32_t right = availableGeometry.x + availableGeometry.width;
    int32_t bottom = availableGeometry.y + availableGeometry.height;
    int32_t x, y;

    if (isRight)
        x = right - m_notificationsMargin;
    else
        x = left + m_notificationsMargin;
    if (isBottom)
        y = bottom - m_notificationsMargin;
    else
        y = top + m_notificationsMargin;

    for (struct weston_surface *surface: m_notificationsLayer) {
        if (isBottom)
            y -= surface->geometry.height;
        if (isRight)
            x -= surface->geometry.width;
        weston_surface_set_position(surface, x, y);
        if (isRight)
            x += surface->geometry.width;
        if (isBottom)
            y -= m_notificationsMargin;
        else
            y += surface->geometry.height + m_notificationsMargin;
    }
}

void DesktopShell::addNotificationSurface(wl_client *client, wl_resource *resource,
                                          wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    surface->geometry.width = weston_surface_buffer_width(surface);
    surface->geometry.height = weston_surface_buffer_height(surface);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<DesktopShell *>(es->configure_private)->notificationConfigure(es, sx, sy, width, height); };
    surface->configure_private = this;
    surface->output = getDefaultOutput();

    if (m_notificationsOrder == DesktopShell::OrderNewestFirst)
        m_notificationsLayer.addSurface(surface);
    else
        m_notificationsLayer.prependSurface(surface);
    mapNotificationSurfaces();

    weston_compositor_schedule_repaint(m_compositor);
}

const struct wl_notification_daemon_interface DesktopShell::m_notificationDaemonImpl = {
    wrapInterface(&DesktopShell::addNotificationSurface)
};

int DesktopShell::screenSaverTimeout()
{
    fadeOut();
    return 1;
}

void DesktopShell::screenSaverSigChild(int status)
{
    m_screenSaverChild.process.pid = 0;
    m_screenSaverChild.client = nullptr; // already destroyed by wayland

    if (m_locked)
        weston_compositor_sleep(m_compositor);
}

void DesktopShell::launchScreenSaverProcess()
{
    if (m_screenSaverBinding)
        return;

    if (m_screenSaverPath.empty()) {
        weston_compositor_sleep(m_compositor);
        return;
    }

    if (m_screenSaverChild.process.pid != 0) {
        weston_log("old screensaver still running\n");
        return;
    }

    m_screenSaverChild.client = weston_client_launch(m_compositor,
                                                     &m_screenSaverChild.process,
                                                     m_screenSaverPath.c_str(),
                                                     [](struct weston_process *process, int status) {
        ScreenSaverChild *child = container_of(process, ScreenSaverChild, process);
        child->shell->screenSaverSigChild(status);
    });

    if (!m_screenSaverChild.client)
        weston_log("not able to start %s\n", m_screenSaverPath.c_str());
}

void DesktopShell::terminateScreenSaverProcess()
{
    if (m_screenSaverChild.process.pid == 0)
        return;

    //::kill(m_screenSaverChild.process.pid, SIGTERM);
}

void DesktopShell::screenSaverConfigure(weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height)
{
    if (width == 0)
        return;

    // Starting screensaver beforehand doesn't work
    if (m_locked)
        return;

    centerSurfaceOnOutput(es, es->output);

    if (wl_list_empty(&es->layer_link)) {
        m_lockLayer.prependSurface(es);
        weston_surface_update_transform(es);
        wl_event_source_timer_update(m_screenSaverTimer, m_screenSaverDuration);
        fadeIn();
    }
}

void DesktopShell::setScreenSaverSurface(wl_client *client, wl_resource *resource,
                                         wl_resource *surface_resource,
                                         wl_resource *output_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    struct weston_output *output = static_cast<weston_output *>(output_resource->data);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {
        static_cast<DesktopShell *>(es->configure_private)->screenSaverConfigure(es, sx, sy, width, height); };
    surface->configure_private = this;
    surface->output = output;
}

const struct screensaver_interface DesktopShell::m_screenSaverImpl = {
    wrapInterface(&DesktopShell::setScreenSaverSurface)
};
