/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/private/qwlsurface_p.h>

#include "compositor.h"
#include "panelmanager.h"
#include "shellpanelsurface.h"
#include "shellsurface.h"

PanelManager::PanelManager(struct ::wl_display *display, QObject *parent)
    : QObject(parent)
    , QtWaylandServer::wl_hawaii_panel_manager(display)
{
}

void PanelManager::hawaii_panel_manager_set_panel(Resource *resource,
                                                  uint32_t id,
                                                  struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    Compositor::instance()->shellSurface()->addSurfaceToLayer(
                Compositor::PanelWindowRole, surface);

    ShellPanelSurface *panelSurface = new ShellPanelSurface(surface);
    panelSurface->init(resource->client(), id);

    connect(surface, &QWaylandSurface::unmapped, [=]() {
        delete panelSurface;
    });
}

#include "moc_panelmanager.cpp"
