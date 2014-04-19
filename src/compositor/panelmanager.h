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

#ifndef PANELMANAGER_H
#define PANELMANAGER_H

#include "qwayland-server-hawaii.h"

class PanelManager : public QObject, public QtWaylandServer::hawaii_panel_manager
{
    Q_OBJECT
public:
    PanelManager(struct ::wl_display *display,
                 QObject *parent = 0);

protected:
    void panel_manager_set_panel(Resource *resource,
                                 uint32_t id,
                                 struct ::wl_resource *surface_resource) Q_DECL_OVERRIDE;
};

#endif // PANELMANAGER_H
