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
#include <weston/compositor.h>

#include "desktop-shell.h"
#include "panelsurface.h"

PanelSurface::PanelSurface(wl_client *client, wl_resource *resource,
                           uint32_t id, weston_surface *surface)
    : m_edge(WL_HAWAII_PANEL_EDGE_BOTTOM)
    , m_alignment(WL_HAWAII_PANEL_ALIGNMENT_LEFT)
    , m_offset(0)
    , m_thickness(0)
    , m_docked(false)
    , m_dockRequested(false)
{
    surfaceListener.listen(&surface->destroy_signal);

    m_view = weston_view_create(surface);

    m_resource = wl_resource_create(client, &wl_hawaii_panel_interface,
                                    wl_resource_get_version(resource), id);
    wl_resource_set_implementation(m_resource, &PanelSurface::implementation, this,
                                   [](struct wl_resource *resource) {});
    wl_resource_set_destructor(m_resource, [](wl_resource *resource) {
        PanelSurface *surface = static_cast<PanelSurface *>(wl_resource_get_user_data(resource));
        delete surface;
    });
    wl_resource_set_user_data(m_resource, this);
}

wl_hawaii_panel_edge PanelSurface::edge() const
{
    return m_edge;
}

weston_view *PanelSurface::view() const
{
    return m_view;
}

weston_output *PanelSurface::output() const
{
    return m_view->output;
}

float PanelSurface::x() const
{
    return m_view->geometry.x;
}

float PanelSurface::y() const
{
    return m_view->geometry.y;
}

int32_t PanelSurface::width() const
{
    return m_view->surface->width;
}

int32_t PanelSurface::height() const
{
    return m_view->surface->height;
}

bool PanelSurface::isDocked() const
{
    return m_docked;
}

void PanelSurface::setAlignment(struct wl_client *,
                                struct wl_resource *,
                                uint32_t alignment)
{
    m_alignment = static_cast<wl_hawaii_panel_alignment>(alignment);
}

void PanelSurface::setOffset(struct wl_client *,
                             struct wl_resource *,
                             uint32_t value)
{
    m_offset = value;
}

void PanelSurface::setThickness(struct wl_client *,
                                struct wl_resource *,
                                uint32_t value)
{
    m_thickness = value;
}

void PanelSurface::setLength(struct wl_client *,
                             struct wl_resource *,
                             uint32_t value)
{
    m_length = value;
}

void PanelSurface::setMinLength(struct wl_client *,
                                struct wl_resource *,
                                uint32_t value)
{
    m_minLength = value;
}

void PanelSurface::setMaxLength(struct wl_client *,
                                struct wl_resource *,
                                uint32_t value)
{
    m_maxLength = value;
}

void PanelSurface::dock(struct wl_client *,
                        struct wl_resource *,
                        uint32_t edge,
                        struct wl_resource *output_resource)
{
    m_edge = static_cast<wl_hawaii_panel_edge>(edge);

    weston_output *output = static_cast<weston_output *>(output_resource->data);
    m_view->output = output;
    m_view->surface->output = output;

    m_dockRequested = true;
}

void PanelSurface::setPosition()
{
    if (!m_dockRequested)
        return;

    movePanel();
    wl_hawaii_panel_send_docked(m_resource);
    m_docked = true;
    m_dockRequested = false;
    dockedSignal(this);
}

void PanelSurface::movePanel()
{
    // Recalculate coordinates
    float x = static_cast<float>(output()->x);
    float y = static_cast<float>(output()->y);

    switch (m_edge) {
    case WL_HAWAII_PANEL_EDGE_LEFT:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<float>(output()->x);
            y = static_cast<float>(output()->y + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = static_cast<float>(output()->x);
            y = static_cast<float>(((output()->y + output()->height) / 2.f) + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<float>(output()->x);
            y = static_cast<float>((output()->y + output()->height) - (m_offset + height()));
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_TOP:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<float>(output()->x) + m_offset;
            y = static_cast<float>(output()->y);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = ((output()->x + output()->width) / 2.f) + (m_offset - (width() / 2.f));
            y = static_cast<float>(output()->y);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<float>((output()->x + output()->width) - (m_offset + width()));
            y = static_cast<float>(output()->y);
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_RIGHT:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<float>((output()->x + output()->width) - width());
            y = static_cast<float>(output()->y + m_offset);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = static_cast<float>(output()->x + output()->width - width());
            y = ((output()->y + output()->height) / 2.f) + (m_offset - height() / 2.f);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<float>(output()->x + output()->width - width());
            y = static_cast<float>((output()->y + output()->height) - (m_offset + height()));
            break;
        }
        break;
    case WL_HAWAII_PANEL_EDGE_BOTTOM:
        switch (m_alignment) {
        case WL_HAWAII_PANEL_ALIGNMENT_LEFT:
            x = static_cast<float>(output()->x + m_offset);
            y = static_cast<float>(output()->y + output()->height - height() + 1);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_CENTER:
            x = ((output()->x + output()->width) / 2.f) + (m_offset - width() / 2.f);
            y = static_cast<float>(output()->y + 1);
            break;
        case WL_HAWAII_PANEL_ALIGNMENT_RIGHT:
            x = static_cast<float>(output()->x + output()->width - (m_offset + width()));
            y = static_cast<float>(output()->y + output()->height - height() + 1);
            break;
        }
        break;
    default:
        break;
    }

    weston_view_set_position(m_view, x, y);
}

const struct wl_hawaii_panel_interface PanelSurface::implementation = {
    wrapInterface(&PanelSurface::setAlignment),
    wrapInterface(&PanelSurface::setOffset),
    wrapInterface(&PanelSurface::setThickness),
    wrapInterface(&PanelSurface::setLength),
    wrapInterface(&PanelSurface::setMinLength),
    wrapInterface(&PanelSurface::setMaxLength),
    wrapInterface(&PanelSurface::dock)
};
