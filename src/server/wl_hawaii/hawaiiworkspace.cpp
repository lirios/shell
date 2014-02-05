/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2014 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include "hawaiiworkspace.h"
#include "workspace.h"

#include "wayland-hawaii-server-protocol.h"

HawaiiWorkspace::HawaiiWorkspace()
    : Interface()
{
}

void HawaiiWorkspace::added()
{
    workspace()->activeChangedSignal.connect(this, &HawaiiWorkspace::activeChanged);
}

void HawaiiWorkspace::init(wl_client *client)
{
    m_resource = wl_resource_create(client, &wl_hawaii_workspace_interface, 1, 0);
    wl_resource_set_implementation(m_resource, &s_implementation, this, 0);
}

Workspace *HawaiiWorkspace::workspace()
{
    return static_cast<Workspace *>(object());
}

HawaiiWorkspace *HawaiiWorkspace::fromResource(wl_resource *res)
{
    return static_cast<HawaiiWorkspace *>(wl_resource_get_user_data(res));
}

void HawaiiWorkspace::activeChanged()
{
    if (workspace()->isActive())
        wl_hawaii_workspace_send_activated(m_resource);
    else
        wl_hawaii_workspace_send_deactivated(m_resource);
}

void HawaiiWorkspace::removed(wl_client *client, wl_resource *res)
{
    object()->destroy();
}

const struct wl_hawaii_workspace_interface HawaiiWorkspace::s_implementation = {
    wrapInterface(&HawaiiWorkspace::removed)
};
