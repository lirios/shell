/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#include <QWaylandCompositor>

#include "extensions/liridecoration.h"
#include "logging_p.h"

LiriDecorationManager::LiriDecorationManager()
    : QWaylandCompositorExtensionTemplate<LiriDecorationManager>()
    , QtWaylandServer::liri_decoration_manager()
{
}

LiriDecorationManager::LiriDecorationManager(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<LiriDecorationManager>(compositor)
    , QtWaylandServer::liri_decoration_manager()
{
}

void LiriDecorationManager::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(lcDecoration) << "Failed to find QWaylandCompositor when initializing LiriDecorationManager";
        return;
    }
    init(compositor->display(), QtWaylandServer::liri_decoration_manager::interfaceVersion());
}

void LiriDecorationManager::unregisterDecoration(LiriDecoration *decoration)
{
    m_decorations.remove(decoration->surface());
}

void LiriDecorationManager::liri_decoration_manager_create(QtWaylandServer::liri_decoration_manager::Resource *resource, uint32_t id, wl_resource *surfaceResource)
{
    auto surface = QWaylandSurface::fromResource(surfaceResource);
    if (!surface) {
        qCWarning(lcDecoration) << "Couldn't find surface";
        return;
    }

    if (m_decorations.contains(surface)) {
        qCWarning(lcDecoration) << "Decoration object already exist for surface";
        wl_resource_post_error(resource->handle, error_already_exists,
                               "liri_decoration already exist for surface");
        return;
    }

    auto decoration = new LiriDecoration(this, surface);
    decoration->init(resource->client(), id, QtWaylandServer::liri_decoration::interfaceVersion());
    m_decorations[surface] = decoration;
    Q_EMIT decorationCreated(decoration);
}

void LiriDecorationManager::liri_decoration_manager_destroy(QtWaylandServer::liri_decoration_manager::Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

LiriDecoration::LiriDecoration(LiriDecorationManager *parent, QWaylandSurface *surface)
    : QtWaylandServer::liri_decoration()
    , m_manager(parent)
    , m_surface(surface)
{
}

LiriDecoration::~LiriDecoration()
{
    m_manager->unregisterDecoration(this);
}

QWaylandSurface *LiriDecoration::surface() const
{
    return m_surface;
}

QColor LiriDecoration::foregroundColor() const
{
    return m_fgColor;
}

QColor LiriDecoration::backgroundColor() const
{
    return m_bgColor;
}

void LiriDecoration::liri_decoration_destroy_resource(QtWaylandServer::liri_decoration::Resource *resource)
{
    Q_UNUSED(resource);
    delete this;
}

void LiriDecoration::liri_decoration_set_foreground(QtWaylandServer::liri_decoration::Resource *resource, const QString &colorName)
{
    Q_UNUSED(resource);

    QColor color(colorName);
    if (m_fgColor == color)
        return;
    m_fgColor = color;
    Q_EMIT foregroundColorChanged(m_fgColor);
}

void LiriDecoration::liri_decoration_set_background(QtWaylandServer::liri_decoration::Resource *resource, const QString &colorName)
{
    Q_UNUSED(resource);

    QColor color(colorName);
    if (m_bgColor == color)
        return;
    m_bgColor = color;
    Q_EMIT backgroundColorChanged(m_bgColor);
}

void LiriDecoration::liri_decoration_destroy(QtWaylandServer::liri_decoration::Resource *resource)
{
    m_manager->unregisterDecoration(this);
    wl_resource_destroy(resource->handle);
}
