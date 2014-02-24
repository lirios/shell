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

#ifndef PANELSURFACE_H
#define PANELSURFACE_H

#include "utils.h"
#include "wayland-hawaii-server-protocol.h"

class DesktopShell;

class PanelSurface : public Interface
{
public:
    PanelSurface(wl_client *client, wl_resource *resource,
                 uint32_t id, weston_surface *surface);

    wl_hawaii_panel_edge edge() const;

    weston_output *output() const;

    float x() const;
    float y() const;

    int32_t width() const;
    int32_t height() const;

    bool isDocked() const;

    void setAlignment(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t alignment);
    void setOffset(struct wl_client *client,
                   struct wl_resource *resource,
                   uint32_t value);
    void setThickness(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t value);
    void setLength(struct wl_client *client,
                   struct wl_resource *resource,
                   uint32_t value);
    void setMinLength(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t value);
    void setMaxLength(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t value);
    void dock(struct wl_client *client,
              struct wl_resource *resource,
              uint32_t edge,
              struct wl_resource *output_resource);

    void setPosition();

    WlListener surfaceListener;
    Signal<PanelSurface *> dockedSignal;

private:
    wl_resource *m_resource;
    weston_view *m_view;
    wl_hawaii_panel_edge m_edge;
    wl_hawaii_panel_alignment m_alignment;
    uint32_t m_offset;
    uint32_t m_thickness;
    uint32_t m_length;
    uint32_t m_minLength;
    uint32_t m_maxLength;
    bool m_docked;
    bool m_dockRequested;

    void movePanel();

    static const struct wl_hawaii_panel_interface implementation;
};

#endif // PANELSURFACE_H
