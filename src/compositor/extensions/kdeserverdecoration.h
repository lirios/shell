/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPLv3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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

#ifndef KDESERVERDECORATION_H
#define KDESERVERDECORATION_H

#include <QWaylandCompositorExtension>

#include "qwayland-server-server-decoration.h"

class QWaylandSurface;

class KdeServerDecorationManager : public QWaylandCompositorExtensionTemplate<KdeServerDecorationManager>
        , public QtWaylandServer::org_kde_kwin_server_decoration_manager
{
    Q_OBJECT
    Q_PROPERTY(Mode defaultMode READ defaultMode WRITE setDefaultMode NOTIFY defaultModeChanged)
public:
    enum Mode {
        None = 0,
        Client,
        Server
    };
    Q_ENUM(Mode)

    KdeServerDecorationManager();
    KdeServerDecorationManager(QWaylandCompositor *compositor);

    void initialize() override;

    Mode defaultMode() const;
    void setDefaultMode(Mode mode);

Q_SIGNALS:
    void defaultModeChanged();
    void decorationCreated(class KdeServerDecoration *decoration);

private:
    Mode m_defaultMode = None;

    void org_kde_kwin_server_decoration_manager_bind_resource(Resource *resource) override;
    void org_kde_kwin_server_decoration_manager_create(Resource *resource,
                                                       uint32_t id,
                                                       struct ::wl_resource *surfaceResource) override;
};

class KdeServerDecoration : public QWaylandCompositorExtensionTemplate<KdeServerDecoration>
        , public QtWaylandServer::org_kde_kwin_server_decoration
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface *surface READ surface CONSTANT)
    Q_PROPERTY(KdeServerDecorationManager::Mode mode READ mode WRITE setMode NOTIFY modeChanged)
public:
    QWaylandSurface *surface() const;

    KdeServerDecorationManager::Mode mode() const;
    void setMode(KdeServerDecorationManager::Mode mode);

Q_SIGNALS:
    void modeChanged();
    void modeRequested(KdeServerDecorationManager::Mode mode);

private:
    KdeServerDecorationManager *m_manager = nullptr;
    QWaylandSurface *m_surface = nullptr;
    KdeServerDecorationManager::Mode m_mode = KdeServerDecorationManager::None;

    explicit KdeServerDecoration(KdeServerDecorationManager *manager, QWaylandSurface *surface);

    void org_kde_kwin_server_decoration_destroy_resource(Resource *resource) override;
    void org_kde_kwin_server_decoration_release(Resource *resource) override;
    void org_kde_kwin_server_decoration_request_mode(Resource *resource, uint32_t mode) override;

    friend class KdeServerDecorationManager;
};

Q_DECLARE_METATYPE(KdeServerDecorationManager::Mode)

#endif // KDESERVERDECORATION_H
