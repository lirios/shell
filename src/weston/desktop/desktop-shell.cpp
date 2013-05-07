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

DesktopShell::DesktopShell(struct weston_compositor *ec)
            : Shell(ec)
{
}

void DesktopShell::init()
{
    Shell::init();

    struct wl_global *global = wl_display_add_global(compositor()->wl_display, &desktop_shell_interface, this,
                                                     [](struct wl_client *client, void *data, uint32_t version, uint32_t id) {
                                                         static_cast<DesktopShell *>(data)->bind(client, version, id);
                                                     });

    if (!global)
        return;

    weston_compositor_add_button_binding(compositor(), BTN_LEFT, MODIFIER_SUPER,
                                         [](struct wl_seat *seat, uint32_t time, uint32_t button, void *data) {
                                             static_cast<DesktopShell *>(data)->moveBinding(seat, time, button);
                                         }, this);

    new ScaleEffect(this);
    new GridDesktops(this);
    new FadeMovingEffect(this);
    new ZoomEffect(this);
}

void DesktopShell::setGrabCursor(uint32_t cursor)
{
    desktop_shell_send_grab_cursor(m_child.desktop_shell, cursor);
}

static void busy_cursor_grab_focus(struct wl_pointer_grab *base, struct wl_surface *surface, int32_t x, int32_t y)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);

    if (grab->grab.focus != surface) {
        Shell::endGrab(grab);
        delete grab;
    }
}

static void busy_cursor_grab_button(struct wl_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state)
{
    ShellGrab *grab = container_of(base, ShellGrab, grab);

    struct weston_surface *surface = container_of(grab->grab.pointer->current, struct weston_surface, surface);
    struct weston_seat *seat = container_of(grab->grab.pointer->seat, struct weston_seat, seat);

    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (shsurf && button == BTN_LEFT && state) {
        grab->shell->activateSurface(shsurf, seat);
        shsurf->dragMove(seat);
    } else if (shsurf && button == BTN_RIGHT && state) {
        grab->shell->activateSurface(shsurf, seat);
//         surface_rotate(shsurf, &seat->seat);
    }
}

static const struct wl_pointer_grab_interface busy_cursor_grab_interface = {
    busy_cursor_grab_focus,
    [](struct wl_pointer_grab *grab, uint32_t time, int32_t x, int32_t y) {},
    busy_cursor_grab_button,
};

void DesktopShell::setBusyCursor(ShellSurface *surface, struct weston_seat *seat)
{
    ShellGrab *grab = new ShellGrab;
    if (!grab)
        return;

    grab->grab.focus = surface->wl_surface();
    startGrab(grab, &busy_cursor_grab_interface, seat, DESKTOP_SHELL_CURSOR_BUSY);
}

void DesktopShell::endBusyCursor(struct weston_seat *seat)
{
    ShellGrab *grab = container_of(seat->pointer.grab, ShellGrab, grab);

    if (grab->grab.interface == &busy_cursor_grab_interface) {
        endGrab(grab);
        delete grab;
    }
}

void DesktopShell::bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    struct wl_resource *resource = wl_client_add_object(client, &desktop_shell_interface, &m_desktop_shell_implementation, id, this);

    if (client == m_child.client) {
        resource->destroy = [](struct wl_resource *resource) { static_cast<DesktopShell *>(resource->data)->unbind(resource); };
        m_child.desktop_shell = resource;
        return;
    }

    wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "permission to bind desktop_shell denied");
    wl_resource_destroy(resource);
}

void DesktopShell::unbind(struct wl_resource *resource)
{
    m_child.desktop_shell = nullptr;
    free(resource);
}

void DesktopShell::moveBinding(struct wl_seat *seat, uint32_t time, uint32_t button)
{
    struct weston_surface *surface = container_of(seat->pointer->focus, struct weston_surface, surface);
    if (!surface) {
        return;
    }

    ShellSurface *shsurf = getShellSurface(surface);
    if (!shsurf || shsurf->type() == ShellSurface::Type::Fullscreen || shsurf->type() == ShellSurface::Type::Maximized) {
        return;
    }

    shsurf = shsurf->topLevelParent();
    if (shsurf) {
        shsurf->dragMove(container_of(seat, struct weston_seat, seat));
    }
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

    desktop_shell_send_configure(resource, 0,
                                 surface_resource,
                                 surface->output->width,
                                 surface->output->height);
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
    desktop_shell_send_configure(resource, 0, surface_resource, surface->output->width, surface->output->height);
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

const struct desktop_shell_interface DesktopShell::m_desktop_shell_implementation = {
    DesktopShell::desktop_shell_set_background,
    DesktopShell::desktop_shell_set_panel,
    DesktopShell::desktop_shell_set_lock_surface,
    DesktopShell::desktop_shell_unlock,
    DesktopShell::desktop_shell_set_grab_surface
};
