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

#ifndef SHELLPANELSURFACE_H
#define SHELLPANELSURFACE_H

#include "qwayland-server-hawaii.h"

class QWaylandSurface;

class ShellPanelSurface : public QtWaylandServer::wl_hawaii_panel
{
public:
    ShellPanelSurface(QWaylandSurface *surface);

protected:
    void hawaii_panel_set_alignment(Resource *resource,
                                    uint32_t alignment) Q_DECL_OVERRIDE;
    void hawaii_panel_set_offset(Resource *resource,
                                 uint32_t offset) Q_DECL_OVERRIDE;
    void hawaii_panel_set_thickness(Resource *resource,
                                    uint32_t thickness) Q_DECL_OVERRIDE;
    void hawaii_panel_set_length(Resource *resource,
                                 uint32_t length) Q_DECL_OVERRIDE;
    void hawaii_panel_set_min_length(Resource *resource,
                                     uint32_t min_length) Q_DECL_OVERRIDE;
    void hawaii_panel_set_max_length(Resource *resource,
                                     uint32_t max_length) Q_DECL_OVERRIDE;
    void hawaii_panel_dock(Resource *resource, uint32_t edge,
                           struct ::wl_resource *output) Q_DECL_OVERRIDE;

private:
    QWaylandSurface *m_surface;
    uint32_t m_alignment;
    uint32_t m_offset;
    uint32_t m_thickness;
    uint32_t m_length;
    uint32_t m_minLength;
    uint32_t m_maxLength;
};

#endif // SHELLPANELSURFACE_H
