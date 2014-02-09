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

#include <QDebug>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>

#include "compositor.h"
#include "shellpanelsurface.h"

ShellPanelSurface::ShellPanelSurface(QWaylandSurface *surface)
    : QtWaylandServer::wl_hawaii_panel()
    , m_surface(surface)
    , m_alignment(WL_HAWAII_PANEL_ALIGNMENT_LEFT)
    , m_offset(0)
    , m_thickness(0)
    , m_length(0)
    , m_minLength(0)
    , m_maxLength(0)
{
}

void ShellPanelSurface::hawaii_panel_set_alignment(Resource *resource,
                                                   uint32_t alignment)
{
    Q_UNUSED(resource);
    m_alignment = alignment;
}

void ShellPanelSurface::hawaii_panel_set_offset(Resource *resource,
                                                uint32_t offset)
{
    Q_UNUSED(resource);
    m_offset = offset;
}

void ShellPanelSurface::hawaii_panel_set_thickness(Resource *resource,
                                                   uint32_t thickness)
{
    Q_UNUSED(resource);
    m_thickness = thickness;
}

void ShellPanelSurface::hawaii_panel_set_length(Resource *resource,
                                                uint32_t length)
{
    Q_UNUSED(resource);
    m_length = length;
}

void ShellPanelSurface::hawaii_panel_set_min_length(Resource *resource,
                                                    uint32_t min_length)
{
    Q_UNUSED(resource);
    m_minLength = min_length;
}

void ShellPanelSurface::hawaii_panel_set_max_length(Resource *resource,
                                                    uint32_t max_length)
{
    Q_UNUSED(resource);
    m_maxLength = max_length;
}

void ShellPanelSurface::hawaii_panel_dock(Resource *resource,
                                          uint32_t edge,
                                          struct ::wl_resource *output)
{
    Q_UNUSED(resource);
    Q_UNUSED(output);

    // FIXME: use output's geometry instead of the primary screen geometry
    QPoint coords = Compositor::instance()->outputGeometry().topLeft();
    QSize size = Compositor::instance()->outputGeometry().size();

    qreal x = static_cast<qreal>(coords.x());
    qreal y = static_cast<qreal>(coords.y());

    int width = m_surface->size().width();
    int height = m_surface->size().height();

    switch (edge) {
    case WL_HAWAII_PANEL_EDGE_LEFT:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<qreal>(coords.x());
            y = static_cast<qreal>(coords.y() + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = static_cast<qreal>(coords.x());
            y = static_cast<qreal>(((coords.y() + size.height()) / 2.f) + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<qreal>(coords.x());
            y = static_cast<qreal>((coords.y() + size.height()) - (m_offset + height));
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_TOP:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<qreal>(coords.x()) + m_offset;
            y = static_cast<qreal>(coords.y());
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = ((coords.x() + size.width()) / 2.f) + (m_offset - (width / 2.f));
            y = static_cast<qreal>(coords.y());
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<qreal>((coords.x() + size.width()) - (m_offset + width));
            y = static_cast<qreal>(coords.y());
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_RIGHT:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<qreal>((coords.x() + size.width()) - width);
            y = static_cast<qreal>(coords.y() + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = static_cast<qreal>(coords.x() + size.width() - width);
            y = ((coords.y() + size.height()) / 2.f) + (m_offset - height / 2.f);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<qreal>(coords.x() + size.width() - width);
            y = static_cast<qreal>((coords.y() + size.height()) - (m_offset + height));
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_BOTTOM:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<qreal>(coords.x() + m_offset);
            y = static_cast<qreal>(coords.y() + size.height() - height + 1);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = ((coords.x() + size.width()) / 2.f) + (m_offset - width / 2.f);
            y = static_cast<qreal>(coords.y() + 1);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<qreal>(coords.x() + size.width() - (m_offset + width));
            y = static_cast<qreal>(coords.y() + size.height() - height + 1);
            break;
        }
        break;
    default:
        break;
    }

    // Move panel surface
    m_surface->setPos(QPointF(x, y));
    if (m_surface->surfaceItem())
        m_surface->surfaceItem()->setPosition(QPointF(x, y));

    // Notify the client
    send_docked();
}
