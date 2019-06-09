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
#include <QWaylandSurface>

#include "kdeserverdecoration.h"
#include "liridecoration.h"

KdeServerDecorationManager::KdeServerDecorationManager()
    : QWaylandCompositorExtensionTemplate<KdeServerDecorationManager>()
    , QtWaylandServer::org_kde_kwin_server_decoration_manager()
{
}

KdeServerDecorationManager::KdeServerDecorationManager(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate<KdeServerDecorationManager>(compositor)
    , QtWaylandServer::org_kde_kwin_server_decoration_manager()
{
}

void KdeServerDecorationManager::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    auto *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(lcDecoration) << "Failed to find QWaylandCompositor when initializing KdeServerDecorationManager";
        return;
    }
    init(compositor->display(), QtWaylandServer::org_kde_kwin_server_decoration_manager::interfaceVersion());
}

KdeServerDecorationManager::Mode KdeServerDecorationManager::defaultMode() const
{
    return m_defaultMode;
}

void KdeServerDecorationManager::setDefaultMode(KdeServerDecorationManager::Mode mode)
{
    if (m_defaultMode == mode)
        return;

    m_defaultMode = mode;
    if (isInitialized())
        send_default_mode(static_cast<uint32_t>(mode));
    Q_EMIT defaultModeChanged();
}

void KdeServerDecorationManager::org_kde_kwin_server_decoration_manager_bind_resource(QtWaylandServer::org_kde_kwin_server_decoration_manager::Resource *resource)
{
    send_default_mode(resource->handle, static_cast<uint32_t>(m_defaultMode));
}

void KdeServerDecorationManager::org_kde_kwin_server_decoration_manager_create(QtWaylandServer::org_kde_kwin_server_decoration_manager::Resource *resource, uint32_t id, wl_resource *surfaceResource)
{
    auto surface = QWaylandSurface::fromResource(surfaceResource);
    if (!surface) {
        qCWarning(lcDecoration) << "Couldn't find surface";
        return;
    }

    auto decoration = new KdeServerDecoration(this, surface);
    decoration->init(resource->client(), id, QtWaylandServer::org_kde_kwin_server_decoration::interfaceVersion());
    decoration->send_mode(static_cast<uint32_t>(m_defaultMode));
    Q_EMIT decorationCreated(decoration);
}

KdeServerDecoration::KdeServerDecoration(KdeServerDecorationManager *manager, QWaylandSurface *surface)
    : QtWaylandServer::org_kde_kwin_server_decoration()
    , m_manager(manager)
    , m_surface(surface)
{
}

QWaylandSurface *KdeServerDecoration::surface() const
{
    return m_surface;
}

KdeServerDecorationManager::Mode KdeServerDecoration::mode() const
{
    return m_mode;
}

void KdeServerDecoration::setMode(KdeServerDecorationManager::Mode mode)
{
    if (m_mode == mode)
        return;
    if (!isInitialized())
        return;

    m_mode = mode;
    Q_EMIT modeChanged();
    send_mode(static_cast<uint32_t>(mode));
}

void KdeServerDecoration::org_kde_kwin_server_decoration_destroy_resource(QtWaylandServer::org_kde_kwin_server_decoration::Resource *resource)
{
    Q_UNUSED(resource);
    delete this;
}

void KdeServerDecoration::org_kde_kwin_server_decoration_release(QtWaylandServer::org_kde_kwin_server_decoration::Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

void KdeServerDecoration::org_kde_kwin_server_decoration_request_mode(QtWaylandServer::org_kde_kwin_server_decoration::Resource *resource, uint32_t mode)
{
    Q_UNUSED(resource);

    m_mode = static_cast<KdeServerDecorationManager::Mode>(mode);
    Q_EMIT modeRequested(m_mode);
    Q_EMIT modeChanged();
}
