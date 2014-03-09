/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
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

#include "desktop-shell.h"
#include "notifications.h"
#include "utils.h"

#include <weston/compositor.h>

Notifications::Notifications()
    : Interface()
    , m_binding(nullptr)
    , m_edge(EdgeRight)
    , m_cornerAnchor(CornerTopRight)
    , m_order(OrderNewestFirst)
    , m_margin(20)
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &wl_notification_daemon_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
        static_cast<Notifications *>(data)->bind(client, version, id);
    });
}

void Notifications::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &wl_notification_daemon_interface, version, id);

    if (m_binding) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "only one client is allowed to bind wl_notification_daemon");
        wl_resource_destroy(resource);
        return;
    }

    wl_resource_set_implementation(resource, &implementation, this,
                                   [](struct wl_resource *resource) {
        static_cast<Notifications *>(wl_resource_get_user_data(resource))->unbind(resource);
    });

    m_binding = resource;
}

void Notifications::unbind(wl_resource *resource)
{
    if (m_binding == resource)
        m_binding = nullptr;
    free(resource);
}

void Notifications::mapSurfaces()
{
    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());

    bool isRight = m_cornerAnchor & EdgeRight;
    bool isBottom = m_cornerAnchor & EdgeBottom;

    weston_output *output = shell->getDefaultOutput();
    IRect2D availableGeometry = shell->windowsArea(output);
    int32_t left = availableGeometry.x;
    int32_t top = availableGeometry.y;
    int32_t right = availableGeometry.x + availableGeometry.width;
    int32_t bottom = availableGeometry.y + availableGeometry.height;
    int32_t x, y;

    if (isRight)
        x = right - m_margin;
    else
        x = left + m_margin;
    if (isBottom)
        y = bottom - m_margin;
    else
        y = top + m_margin;

    for (weston_view *view: m_notificationsLayer) {
        if (isBottom)
            y -= view->surface->height;
        if (isRight)
            x -= view->surface->width;
        weston_view_set_position(view, x, y);
        if (isRight)
            x += view->surface->width;
        if (isBottom)
            y -= m_margin;
        else
            y += view->surface->height + m_margin;
    }

    weston_compositor_schedule_repaint(shell->compositor());
}

void Notifications::addSurface(wl_client *client,
                               wl_resource *resource,
                               wl_resource *surface_resource)
{
    weston_surface *surface = static_cast<weston_surface *>(
                wl_resource_get_user_data(surface_resource));
    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return;
    }

    weston_view *view, *next;
    wl_list_for_each_safe(view, next, &surface->views, surface_link)
            weston_view_destroy(view);
    view = weston_view_create(surface);
    view->output = Shell::instance()->getDefaultOutput();

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        Notifications *notifications = static_cast<Notifications *>(es->configure_private);
        notifications->mapSurfaces();
    };
    surface->configure_private = this;
    surface->output = view->output;

    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());
    if (m_order == OrderNewestFirst)
        m_notificationsLayer.addSurface(view);
    else
        m_notificationsLayer.prependSurface(view);
    mapSurfaces();
}

const struct wl_notification_daemon_interface Notifications::implementation = {
    [](wl_client *client, wl_resource *resource, wl_resource *surface_resource) {
        static_cast<Notifications *>(wl_resource_get_user_data(resource))->addSurface(client, resource, surface_resource);
    }
};
