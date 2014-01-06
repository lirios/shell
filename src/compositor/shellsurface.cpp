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

#include <QtCompositor/QWaylandInputDevice>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/private/qwlsurface_p.h>

#include "shellpopupsurface.h"
#include "shellsurface.h"

ShellSurface::ShellSurface(wl_display *display)
    : QtWaylandServer::wl_hawaii_shell_surface(display)
{
}

void ShellSurface::setSurfacesVisible(bool visible)
{
    for (QWaylandSurface *surface: m_panelsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(visible);
    }
    for (QWaylandSurface *surface: m_overlayLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(visible);
    }
    for (QWaylandSurface *surface: m_dialogsLayer) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setVisible(visible);
    }
}

QWaylandSurface *ShellSurface::surfaceAt(const QPointF &point, QPointF *local)
{
    QWaylandSurface *surface;

#define SURFACE_AT(layer) \
    surface = surfaceAt(layer, point, local); \
    if (surface) \
    return surface;

    SURFACE_AT(m_dialogsLayer);
    //SURFACE_AT(m_overlayLayer);
    SURFACE_AT(m_panelsLayer);
    //SURFACE_AT(m_backgroundLayer);

#undef SURFACE_AT

    return 0;
}

void ShellSurface::hawaii_shell_surface_set_background(Resource *resource,
                                                       struct ::wl_resource *output_resource,
                                                       struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::BackgroundWindowRole, surface);
}

void ShellSurface::hawaii_shell_surface_set_panel(Resource *resource,
                                                  struct ::wl_resource *output_resource,
                                                  struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::PanelWindowRole, surface);
}

void ShellSurface::hawaii_shell_surface_set_overlay(Resource *resource,
                                                    struct ::wl_resource *output_resource,
                                                    struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::OverlayWindowRole, surface);
}

void ShellSurface::hawaii_shell_surface_set_popup(Resource *resource,
                                                  uint32_t id,
                                                  struct ::wl_resource *output_resource,
                                                  struct ::wl_resource *surface_resource,
                                                  int32_t x, int32_t y)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    if (m_panelsLayer.contains(surface))
        return;
    surface->setWindowProperty(QStringLiteral("position"), QPointF(x, y));
    addSurfaceToLayer(Compositor::PopupWindowRole, surface);

    QtWayland::InputDevice *input = Compositor::instance()->defaultInputDevice()->handle();

    ShellPopupSurface *popupSurface = new ShellPopupSurface(surface);
    popupSurface->init(resource->client(), id);

    PopupGrabber *grabber = new PopupGrabber(input);
    wl_resource_set_user_data(popupSurface->resource()->handle, grabber);
    popupSurface->setPopupGrabber(grabber);
}

void ShellSurface::hawaii_shell_surface_set_dialog(Resource *resource,
                                                   struct ::wl_resource *output_resource,
                                                   struct ::wl_resource *surface_resource)
{
    Q_UNUSED(resource);
    Q_UNUSED(output_resource);

    QWaylandSurface *surface =
            QtWayland::Surface::fromResource(surface_resource)->waylandSurface();
    surface->setWindowProperty(QStringLiteral("position"), surface->pos());
    addSurfaceToLayer(Compositor::DialogWindowRole, surface);
}

void ShellSurface::addSurfaceToLayer(Compositor::ShellWindowRole role, QWaylandSurface *surface)
{
    if (!surface)
        return;

    surface->setWindowProperty(QStringLiteral("role"), role);

    switch (role) {
    case Compositor::BackgroundWindowRole:
        m_backgroundLayer.append(surface);
        break;
    case Compositor::PanelWindowRole:
    case Compositor::PopupWindowRole:
        m_panelsLayer.append(surface);
        break;
    case Compositor::OverlayWindowRole:
        m_overlayLayer.append(surface);
        break;
    case Compositor::DialogWindowRole:
        m_dialogsLayer.append(surface);
        break;
    default:
        break;
    }

    connect(surface, &QWaylandSurface::unmapped, [=]() {
        removeSurfaceFromLayer(surface);
    });
}

void ShellSurface::removeSurfaceFromLayer(QWaylandSurface *surface)
{
    if (!surface)
        return;

    bool found = false;
    Compositor::ShellWindowRole role = static_cast<Compositor::ShellWindowRole>(
                surface->windowProperties().value(QStringLiteral("role")).toInt(&found));
    if (!found)
        return;

    switch (role) {
    case Compositor::BackgroundWindowRole:
        m_backgroundLayer.removeOne(surface);
        break;
    case Compositor::PanelWindowRole:
    case Compositor::PopupWindowRole:
        m_panelsLayer.removeOne(surface);
        break;
    case Compositor::OverlayWindowRole:
        m_overlayLayer.removeOne(surface);
        break;
    case Compositor::DialogWindowRole:
        m_dialogsLayer.removeOne(surface);
        break;
    default:
        break;
    }
}

QWaylandSurface *ShellSurface::surfaceAt(const Layer &layer, const QPointF &point, QPointF *local)
{
    // Iterate through the layer in reverse order
    for (int i = layer.size() - 1; i >= 0; --i) {
        QWaylandSurface *surface = layer.at(i);

        // Ignore hidden surfaces or surfaces not mapped
        if (!surface->visible() || !surface->surfaceItem())
            continue;

        // Are the coordinates inside an item?
        QWaylandSurfaceItem *item = surface->surfaceItem();
        QRectF geo(item->position(), QSizeF(item->width(), item->height()));
        if (geo.contains(point)) {
            if (local)
                *local = point - item->position();
            return surface;
        }
    }

    return 0;
}

#include "moc_shellsurface.cpp"
