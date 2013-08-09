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

#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>

#include <wayland-server.h>

#include <weston/compositor.h>
#include <weston/matrix.h>
// #include <weston/config-parser.h>

#include "desktop-shell.h"
#include "wayland-desktop-shell-server-protocol.h"
#include "shellsurface.h"

#include "scaleeffect.h"
#include "griddesktops.h"
#include "fademovingeffect.h"
#include "zoomeffect.h"
#include "inputpanel.h"
#include "shellseat.h"
#include "workspace.h"

DesktopShell::DesktopShell(struct weston_compositor *ec)
    : Shell(ec)
{
}

void DesktopShell::init()
{
    Shell::init();

    if (!wl_global_create(compositor()->wl_display, &desktop_shell_interface, 1, this,
        [](struct wl_client *client, void *data, uint32_t version, uint32_t id) { static_cast<DesktopShell *>(data)->bind(client, version, id); }))
        return;

    weston_compositor_add_button_binding(compositor(), BTN_LEFT, MODIFIER_SUPER,
                                         [](struct weston_seat *seat, uint32_t time, uint32_t button, void *data) {
                                             static_cast<DesktopShell *>(data)->moveBinding(seat, time, button);
                                         }, this);

    new ScaleEffect(this);
    new GridDesktops(this);
    new FadeMovingEffect(this);
    new ZoomEffect(this);

    m_inputPanel = new InputPanel(compositor()->wl_display);
}

void DesktopShell::setGrabCursor(uint32_t cursor)
{
    hawaii_desktop_shell_send_grab_cursor(m_child.desktop_shell, cursor);
}

static void busy_cursor_grab_focus(struct weston_pointer_grab *base)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);
    wl_fixed_t sx, sy;
    struct weston_surface *surface = weston_compositor_pick_surface(grab->pointer->seat->compositor,
                                                                    grab->pointer->x, grab->pointer->y,
                                                                    &sx, &sy);

    if (grab->pointer->focus != surface) {
        Shell::endGrab(grab);
        delete grab;
    }
}

static void busy_cursor_grab_button(struct weston_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);

    struct weston_surface *surface = grab->pointer->focus;
    struct weston_seat *seat = grab->pointer->seat;

    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (shsurf && button == BTN_LEFT && state) {
        ShellSeat::shellSeat(seat)->activate(shsurf);
        shsurf->dragMove(seat);
    } else if (shsurf && button == BTN_RIGHT && state) {
        ShellSeat::shellSeat(seat)->activate(shsurf);
        //         surface_rotate(shsurf, &seat->seat);
    }
}

static const struct weston_pointer_grab_interface busy_cursor_grab_interface = {
    busy_cursor_grab_focus,
    [](struct weston_pointer_grab *grab, uint32_t time) {},
    busy_cursor_grab_button,
};

void DesktopShell::setBusyCursor(ShellSurface *surface, struct weston_seat *seat)
{
    ShellGrab *grab = new ShellGrab;
    if (!grab && grab->pointer)
        return;

    grab->pointer = seat->pointer;
    grab->pointer->focus = surface->weston_surface();
    startGrab(grab, &busy_cursor_grab_interface, seat, DESKTOP_SHELL_CURSOR_BUSY);
}

void DesktopShell::endBusyCursor(struct weston_seat *seat)
{
    ShellGrab *grab = container_of(seat->pointer->grab, ShellGrab, grab);

    if (grab->grab.interface == &busy_cursor_grab_interface) {
        endGrab(grab);
        delete grab;
    }
}

void DesktopShell::bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_resource_create(client, &desktop_shell_interface, version, id);

    if (client == m_child.client) {
        wl_resource_set_implementation(resource, &m_desktop_shell_implementation, this,
                                       [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbind(resource); });
        m_child.desktop_shell = resource;
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind desktop_shell denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbind(struct wl_resource *resource)
{
    m_child.desktop_shell = nullptr;
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

void DesktopShell::setAvailableGeometry(struct wl_client *client, struct wl_resource *resource,
                                        struct wl_resource *output_resource,
                                        int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct weston_output *output = static_cast<weston_output *>(output_resource->data);

    IRect2D area;
    area.x = x;
    area.y = y;
    area.width = width;
    area.height = height;

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
    hawaii_desktop_shell_send_present(resource, surface_resource);
}

static void panelAnimationDone(struct weston_surface_animation *,
                               void *data)
{
    struct animation_coordinates *coords = static_cast<animation_coordinates *>(data);
    weston_surface_set_position(coords->surface, coords->x, coords->y);
    free(data);
}

void DesktopShell::setPanel(struct wl_client *client, struct wl_resource *resource,
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
    hawaii_desktop_shell_send_present(resource, surface_resource);
}

void DesktopShell::setPanelGeometry(struct wl_client *client, struct wl_resource *resource,
                                    struct wl_resource *output_resource,
                                    struct wl_resource *surface_resource,
                                    int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);
    struct weston_output *output = static_cast<struct weston_output *>(output_resource->data);

    struct animation_coordinates *coords =
            (struct animation_coordinates *)malloc(sizeof(struct animation_coordinates));
            coords->surface = surface;
            coords->x = x;
            coords->y = y;

            weston_slide_run(surface, -output->height, 0, panelAnimationDone, coords);
}

void DesktopShell::setLauncher(struct wl_client *client, struct wl_resource *resource,
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

    addLauncherSurface(surface, static_cast<weston_output *>(output_resource->data));
    hawaii_desktop_shell_send_present(resource, surface_resource);
}

void DesktopShell::setLauncherGeometry(struct wl_client *client, struct wl_resource *resource,
                                       struct wl_resource *output_resource,
                                       struct wl_resource *surface_resource,
                                       int32_t x, int32_t y, int32_t width, int32_t height)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);
    struct weston_output *output = static_cast<struct weston_output *>(output_resource->data);

    struct animation_coordinates *coords =
            (struct animation_coordinates *)malloc(sizeof(struct animation_coordinates));
            coords->surface = surface;
            coords->x = x;
            coords->y = y;

            weston_slide_run(surface, output->height, y, panelAnimationDone, coords);
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

    addSpecialSurface(surface, static_cast<weston_output *>(output_resource->data));
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

    // When we hide a special surface we unmap and unlink it, so the next
    // time set_position() is called we need to insert it into the
    // panel layer again
    bool found = false;
    for (struct weston_surface *current: m_panelsLayer) {
        if (current == surface) {
            found = true;
            break;
        }
    }
    if (!found)
        m_panelsLayer.addSurface(surface);

#if 0
    // Give focus to the special surface
    if (!shell->locked) {
        struct weston_seat *seat;

        wl_list_for_each(seat, &compositor->seat_list, link)
                weston_surface_activate(surface, seat);
    }
#else
    struct weston_seat *seat;
    wl_list_for_each(seat, &m_compositor->seat_list, link)
            weston_surface_activate(surface, seat);
#endif

    // Set given position
    weston_surface_set_position(surface, x, y);
}

void DesktopShell::hideSurface(struct wl_client *client, struct wl_resource *resource,
                               struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<struct weston_surface *>(surface_resource->data);

    if (!surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role not yet assigned");
        return;
    }

    for (struct weston_surface *current: m_panelsLayer) {
        if (current == surface) {
            weston_surface_unmap(surface);
            wl_list_remove(&surface->link);
        }
    }
}

void DesktopShell::setLockSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
//     struct desktop_shell *shell = resource->data;
//     struct weston_surface *surface = surface_resource->data;
//
//     shell->prepare_event_sent = false;
//
//     if (!shell->locked)
//         return;
//
//     shell->lock_surface = surface;
//
//     shell->lock_surface_listener.notify = handle_lock_surface_destroy;
//     wl_signal_add(&surface_resource->destroy_signal,
//                   &shell->lock_surface_listener);
//
//     surface->configure = lock_surface_configure;
//     surface->configure_private = shell;
}

void DesktopShell::unlock(struct wl_client *client, struct wl_resource *resource)
{
//     struct desktop_shell *shell = resource->data;
//
//     shell->prepare_event_sent = false;
//
//     if (shell->locked)
//         resume_desktop(shell);
}

void DesktopShell::setGrabSurface(struct wl_client *client, struct wl_resource *resource, struct wl_resource *surface_resource)
{
    this->Shell::setGrabSurface(static_cast<struct weston_surface *>(surface_resource->data));
}

void DesktopShell::addKeyBinding(struct wl_client *client, struct wl_resource *resource, uint32_t id, uint32_t key, uint32_t modifiers)
{
    wl_resource *res = wl_resource_create(client, &desktop_shell_binding_interface, wl_resource_get_version(resource), id);
    wl_resource_set_implementation(res, nullptr, res, [](wl_resource *) {});

    weston_compositor_add_key_binding(compositor(), key, (weston_keyboard_modifier)modifiers,
                                         [](struct weston_seat *seat, uint32_t time, uint32_t key, void *data) {
                                             desktop_shell_binding_send_triggered(static_cast<wl_resource *>(data));
                                         }, res);
}

void DesktopShell::addOverlay(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output_resource, struct wl_resource *surface_resource)
{
    struct weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    addOverlaySurface(surface, static_cast<weston_output *>(output_resource->data));
    desktop_shell_send_configure(resource, 0, surface_resource, surface->output->width, surface->output->height);
    pixman_region32_fini(&surface->pending.input);
    pixman_region32_init_rect(&surface->pending.input, 0, 0, 0, 0);
}

void DesktopShell::requestFocus(wl_client *client, wl_resource *resource, wl_resource *surface_resource)
{
    weston_surface *surf = static_cast<weston_surface *>(surface_resource->data);
    weston_seat *seat;
    wl_list_for_each(seat, &compositor()->seat_list, link)
        ShellSeat::shellSeat(seat)->activate(surf);
}

void DesktopShell::minimizeWindows(wl_client *client, wl_resource *resource)
{
    selectWorkspace(-1);
}

void DesktopShell::restoreWindows(wl_client *client, wl_resource *resource)
{
    selectWorkspace(currentWorkspace()->number());
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
        desktop_shell_grab_send_focus(cgrab->resource, surface->resource, sx, sy);
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

    desktop_shell_grab_send_motion(cgrab->resource, time, sx, sy);
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

    desktop_shell_grab_send_button(cgrab->resource, time, button, state);
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

static const struct desktop_shell_grab_interface desktop_shell_grab_implementation = {
    client_grab_end
};

void DesktopShell::createGrab(wl_client *client, wl_resource *resource, uint32_t id)
{
    wl_resource *res = wl_resource_create(client, &desktop_shell_grab_interface, wl_resource_get_version(resource), id);

    ClientGrab *grab = new ClientGrab;
    wl_resource_set_implementation(res, &desktop_shell_grab_implementation, grab, [](wl_resource *) {});

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
    desktop_shell_grab_send_focus(grab->resource, surface->resource, sx, sy);

    grab->grab.interface = &client_grab_interface;
    weston_pointer_start_grab(grab->pointer, &grab->grab);
}

void DesktopShell::quit(wl_client *client, wl_resource *resource)
{
    Shell::quit();
}

static void
desktop_shell_desktop_ready(struct wl_client *client,
                            struct wl_resource *resource)
{
}

const struct desktop_shell_interface DesktopShell::m_desktop_shell_implementation = {
    DesktopShell::hawaii_desktop_shell_set_available_geometry,
    DesktopShell::hawaii_desktop_shell_set_background,
    DesktopShell::hawaii_desktop_shell_set_panel,
    DesktopShell::hawaii_desktop_shell_set_panel_geometry,
    DesktopShell::hawaii_desktop_shell_set_launcher,
    DesktopShell::hawaii_desktop_shell_set_launcher_geometry,
    DesktopShell::hawaii_desktop_shell_set_special,
    DesktopShell::hawaii_desktop_shell_set_position,
    DesktopShell::hawaii_desktop_shell_hide_surface,
    DesktopShell::hawaii_desktop_shell_set_lock_surface,
    DesktopShell::hawaii_desktop_shell_quit,
    DesktopShell::hawaii_desktop_shell_lock,
    DesktopShell::hawaii_desktop_shell_unlock,
    DesktopShell::hawaii_desktop_shell_set_grab_surface
    DesktopShell::desktop_shell_add_key_binding,
    DesktopShell::desktop_shell_add_overlay,
    DesktopShell::desktop_shell_request_focus,
    DesktopShell::desktop_shell_minimize_windows,
    DesktopShell::desktop_shell_restore_windows,
    DesktopShell::desktop_shell_create_grab
};
