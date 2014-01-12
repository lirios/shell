/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLSURFACE_H
#define SHELLSURFACE_H

#include <QtCompositor/QWaylandSurface>

#include "compositor.h"
#include "qwayland-server-hawaii.h"

typedef QList<QWaylandSurface *> Layer;

class ShellSurface : public QObject, public QtWaylandServer::wl_hawaii_shell_surface
{
    Q_OBJECT
public:
    ShellSurface(struct ::wl_display *display);

    void setSurfacesVisible(bool visible);

    QWaylandSurface *surfaceAt(const QPointF &point, QPointF *local);

protected:
    void hawaii_shell_surface_set_background(Resource *resource,
                                             struct ::wl_resource *output_resource,
                                             struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_surface_set_panel(Resource *resource,
                                        struct ::wl_resource *output_resource,
                                        struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_surface_set_overlay(Resource *resource,
                                          struct ::wl_resource *output_resource,
                                          struct ::wl_resource *surface) Q_DECL_OVERRIDE;
    void hawaii_shell_surface_set_popup(Resource *resource, uint32_t id,
                                        struct ::wl_resource *output_resource,
                                        struct ::wl_resource *parent_resource,
                                        struct ::wl_resource *surface_resource,
                                        int32_t x, int32_t y) Q_DECL_OVERRIDE;
    void hawaii_shell_surface_set_dialog(Resource *resource,
                                         struct ::wl_resource *output_resource,
                                         struct ::wl_resource *surface) Q_DECL_OVERRIDE;

private:
    Q_DISABLE_COPY(ShellSurface)

    Layer m_backgroundLayer;
    Layer m_panelsLayer;
    Layer m_overlayLayer;
    Layer m_dialogsLayer;

    void addSurfaceToLayer(Compositor::ShellWindowRole role, QWaylandSurface *surface);
    void removeSurfaceFromLayer(QWaylandSurface *surface);

    QWaylandSurface *surfaceAt(const Layer &layer, const QPointF &point, QPointF *local);
};

#endif // SHELLSURFACE_H
