/*
 * Copyright 2014 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include "xdgshell.h"
#include "shell.h"
#include "shellsurface.h"
#include "xdgsurface.h"
#include "shellseat.h"
#include "utils.h"

#include "wayland-xdg-shell-server-protocol.h"

XdgShell::XdgShell()
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &xdg_shell_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
                         static_cast<XdgShell *>(data)->bind(client, version, id);
                     });

    weston_seat *seat;
    wl_list_for_each(seat, &Shell::compositor()->seat_list, link) {
        ShellSeat *shseat = ShellSeat::shellSeat(seat);
        shseat->pointerFocusSignal.connect(this, &XdgShell::pointerFocus);
        shseat->keyboardFocusSignal.connect(this, &XdgShell::keyboardFocus);
    }
}

void XdgShell::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &xdg_shell_interface, version, id);
    if (resource)
        wl_resource_set_implementation(resource, &shell_implementation, this, nullptr);
}

void XdgShell::sendConfigure(weston_surface *surface, int32_t width, int32_t height)
{
    XdgSurface *surf = static_cast<XdgSurface *>(surface->configure_private);
    xdg_surface_send_configure(surf->resource(), (uint32_t)surf->shsurf()->resizeEdges(), width, height);
}

void XdgShell::useUnstableVersion(wl_client *client, wl_resource *resource, int32_t version)
{

}

ShellSurface *XdgShell::getXdgSurface(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource)
{
    weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));

    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "XdgShell::getShellSurface already requested");
        return shsurf;
    }

    shsurf = Shell::instance()->createShellSurface(surface, &shell_client);
    if (!shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "surface->configure already set");
        return nullptr;
    }

    shsurf->setTopLevel();
    XdgSurface *xdg = new XdgSurface(this);
    surface->configure_private = xdg;
    shsurf->addInterface(xdg);
    xdg->init(client, id);
    xdg->responsivenessChangedSignal.connect(this, &XdgShell::surfaceResponsiveness);

    return shsurf;
}

void XdgShell::getXdgPopup(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource, wl_resource *parentRes,
                           wl_resource *seatRes, uint32_t serial, int32_t x, int32_t y, uint32_t flags)
{
    weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));

    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "XdgShell::getShellSurface already requested");
        return;
    }

    shsurf = Shell::instance()->createShellSurface(surface, &shell_client);
    if (!shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "surface->configure already set");
        return;
    }

    weston_surface *parent = static_cast<weston_surface *>(wl_resource_get_user_data(parentRes));
    weston_seat *seat = static_cast<weston_seat *>(wl_resource_get_user_data(seatRes));

    shsurf->setPopup(parent, seat, x, y, serial);
    XdgPopup *xdg = new XdgPopup(this, serial);
    surface->configure_private = xdg;
    shsurf->addInterface(xdg);
    xdg->init(client, id);
    xdg->responsivenessChangedSignal.connect(this, &XdgShell::surfaceResponsiveness);
}

void XdgShell::pointerFocus(ShellSeat *, weston_pointer *pointer)
{
    weston_view *view = pointer->focus;

    if (!view)
        return;

    ShellSurface *shsurf = Shell::getShellSurface(view->surface);
    if (!shsurf)
        return;

    XdgBaseSurface *xdg = shsurf->findInterface<XdgBaseSurface>();
    if (!xdg) {
        return;
    }

    if (!xdg->isResponsive()) {
        surfaceResponsivenessChangedSignal(shsurf, false);
    } else {
        uint32_t serial = wl_display_next_serial(Shell::compositor()->wl_display);
        xdg->ping(serial);
    }
}

void XdgShell::keyboardFocus(ShellSeat *seat, weston_keyboard *keyboard)
{
    if (seat->currentKeyboardFocus()) {
        ShellSurface *shsurf = Shell::getShellSurface(seat->currentKeyboardFocus());
        if (shsurf) {
            XdgSurface *xdg = shsurf->findInterface<XdgSurface>();
            if (xdg) {
                xdg->loseFocus();
            }
        }
    }

    if (keyboard->focus) {
        ShellSurface *shsurf = Shell::getShellSurface(keyboard->focus);
        if (shsurf) {
            XdgSurface *xdg = shsurf->findInterface<XdgSurface>();
            if (xdg) {
                xdg->gainFocus();
            }
        }
    }
}

void XdgShell::surfaceResponsiveness(XdgBaseSurface *surf)
{
    ShellSurface *shsurf = surf->shsurf();
    surfaceResponsivenessChangedSignal(shsurf, surf->isResponsive());
}

const weston_shell_client XdgShell::shell_client = {
    XdgShell::sendConfigure
};

const struct xdg_shell_interface XdgShell::shell_implementation = {
    wrapInterface(&XdgShell::useUnstableVersion),
    wrapInterface(&XdgShell::getXdgSurface),
    wrapInterface(&XdgShell::getXdgPopup)
};
