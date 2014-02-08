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

#include "wlshell.h"
#include "shell.h"
#include "shellsurface.h"
#include "wlshellsurface.h"
#include "shellseat.h"
#include "utils.h"

WlShell::WlShell()
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &wl_shell_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
                         static_cast<WlShell *>(data)->bind(client, version, id);
                     });

    weston_seat *seat;
    wl_list_for_each(seat, &Shell::compositor()->seat_list, link) {
        ShellSeat *shseat = ShellSeat::shellSeat(seat);
        shseat->pointerFocusSignal.connect(this, &WlShell::pointerFocus);
    }
}

void WlShell::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &wl_shell_interface, version, id);
    if (resource)
        wl_resource_set_implementation(resource, &shell_implementation, this, nullptr);
}

void WlShell::sendConfigure(weston_surface *surface, uint32_t edges, int32_t width, int32_t height)
{
    WlShellSurface *wlss = static_cast<WlShellSurface *>(surface->configure_private);
    wl_shell_surface_send_configure(wlss->resource(), edges, width, height);
}

ShellSurface *WlShell::getShellSurface(wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource)
{
    weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surface_resource));

    ShellSurface *shsurf = Shell::getShellSurface(surface);
    if (shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "WlShell::getShellSurface already requested");
        return shsurf;
    }

    shsurf = Shell::instance()->createShellSurface(surface, &shell_client);
    if (!shsurf) {
        wl_resource_post_error(surface_resource, WL_DISPLAY_ERROR_INVALID_OBJECT, "surface->configure already set");
        return nullptr;
    }

    WlShellSurface *wlss = new WlShellSurface(this);
    surface->configure_private = wlss;
    shsurf->addInterface(wlss);
    wlss->init(client, id);
    wlss->responsivenessChangedSignal.connect(this, &WlShell::surfaceResponsiveness);

    return shsurf;
}

void WlShell::pointerFocus(ShellSeat *, weston_pointer *pointer)
{
    weston_view *view = pointer->focus;

    if (!view)
        return;

    ShellSurface *shsurf = Shell::getShellSurface(view->surface);
    if (!shsurf)
        return;

    WlShellSurface *wlss = shsurf->findInterface<WlShellSurface>();
    if (!wlss) {
        return;
    }

    if (!wlss->isResponsive()) {
        surfaceResponsivenessChangedSignal(shsurf, false);
    } else {
        uint32_t serial = wl_display_next_serial(Shell::compositor()->wl_display);
        wlss->ping(serial);
    }
}

void WlShell::surfaceResponsiveness(WlShellSurface *wlsurf)
{
    ShellSurface *shsurf = wlsurf->shsurf();
    surfaceResponsivenessChangedSignal(shsurf, wlsurf->isResponsive());
}

const weston_shell_client WlShell::shell_client = {
    WlShell::sendConfigure
};

const struct wl_shell_interface WlShell::shell_implementation = {
    wrapInterface(&WlShell::getShellSurface)
};
