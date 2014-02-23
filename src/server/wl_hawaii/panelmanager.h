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

#ifndef PANELMANAGER_H
#define PANELMANAGER_H

#include "interface.h"
#include "panelsurface.h"

class PanelManager : public Interface
{
public:
    PanelManager();

    std::list<PanelSurface *> panels() const;

private:
    void bind(wl_client *client, uint32_t version, uint32_t id);
    void unbind(wl_resource *resource);

    PanelSurface *getPanelSurface(wl_client *client,
                                  wl_resource *resource,
                                  uint32_t id,
                                  wl_resource *surface_resource);

    void surfaceDestroyed(void *);

    wl_resource *m_binding;
    std::list<PanelSurface *> m_panels;

    static const struct wl_hawaii_panel_manager_interface implementation;
};

#endif // PANELMANAGER_H
