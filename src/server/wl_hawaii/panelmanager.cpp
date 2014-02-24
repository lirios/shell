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

#include <wayland-server.h>

#include "desktop-shell.h"
#include "panelmanager.h"

PanelManager::PanelManager()
    : Interface()
    , m_binding(nullptr)
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &wl_hawaii_panel_manager_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
        static_cast<PanelManager *>(data)->bind(client, version, id);
    });
}

std::list<PanelSurface *> PanelManager::panels() const
{
    return m_panels;
}

void PanelManager::bind(wl_client *client, uint32_t version, uint32_t id)
{
    wl_resource *resource = wl_resource_create(client, &wl_hawaii_panel_manager_interface, version, id);

    if (m_binding) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "only one client is allowed to bind wl_hawaii_panel_manager");
        wl_resource_destroy(resource);
        return;
    }

    if (client != Shell::instance()->shellClient()) {
        wl_resource_post_error(resource, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "permission to bind wl_hawaii_panel_manager denied");
        wl_resource_destroy(resource);
        return;
    }

    wl_resource_set_implementation(resource, &implementation, this,
                                   [](struct wl_resource *resource) {
        static_cast<PanelManager *>(resource->data)->unbind(resource);
    });

    m_binding = resource;
}

void PanelManager::unbind(wl_resource *resource)
{
    if (m_binding == resource)
        m_binding = nullptr;
}

PanelSurface *PanelManager::getPanelSurface(wl_client *client,
                                            wl_resource *resource,
                                            uint32_t id,
                                            wl_resource *surface_resource)
{
    weston_surface *surface = static_cast<weston_surface *>(surface_resource->data);

    if (surface->configure) {
        wl_resource_post_error(surface_resource,
                               WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "surface role already assigned");
        return nullptr;
    }

    PanelSurface *panel = new PanelSurface(client, resource, id, surface);
    m_panels.push_back(panel);

    panel->surfaceListener.signal->connect(this, &PanelManager::surfaceDestroyed);
    panel->dockedSignal.connect(this, &PanelManager::panelDocked);

    surface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {
        DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());
        if (!shell)
            return;

        PanelSurface *panel = static_cast<PanelSurface *>(es->configure_private);
        if (!panel)
            return;

        weston_view *view = container_of(es->views.next, weston_view, surface_link);
        shell->addPanelSurfaceToLayer(view);
        panel->setPosition();
    };
    surface->configure_private = panel;
    surface->output = nullptr;

    return panel;
}

void PanelManager::surfaceDestroyed(void *d)
{
    PanelSurface *panel = static_cast<PanelSurface *>(d);
    if (panel) {
        panel->surfaceListener.reset();
        m_panels.remove(panel);
    }
}

void PanelManager::panelDocked(PanelSurface *)
{
    DesktopShell *shell = static_cast<DesktopShell *>(Shell::instance());
    if (!shell)
        return;

    shell->recalculateAvailableGeometry();
}

const struct wl_hawaii_panel_manager_interface PanelManager::implementation = {
    [](wl_client *client, wl_resource *resource, uint32_t id, wl_resource *surface_resource) {
        static_cast<PanelManager *>(wl_resource_get_user_data(resource))->getPanelSurface(client, resource, id, surface_resource);
    }
};
