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

#ifndef LIRIDECORATION_H
#define LIRIDECORATION_H

#include <QLoggingCategory>
#include <QPointer>
#include <QWaylandCompositorExtension>
#include <QWaylandResource>
#include <QWaylandSurface>

#include "qwayland-server-liri-decoration.h"

Q_DECLARE_LOGGING_CATEGORY(lcDecoration)

class LiriDecoration;

class LiriDecorationManager : public QWaylandCompositorExtensionTemplate<LiriDecorationManager>
        , public QtWaylandServer::liri_decoration_manager
{
    Q_OBJECT
public:
    LiriDecorationManager();
    LiriDecorationManager(QWaylandCompositor *compositor);

    void initialize() override;

    void unregisterDecoration(LiriDecoration *decoration);

Q_SIGNALS:
    void decorationCreated(LiriDecoration *decoration);

private:
    QMap<QWaylandSurface *, LiriDecoration *> m_decorations;

    void liri_decoration_manager_create(Resource *resource, uint32_t id, struct ::wl_resource *surfaceResource) override;
    void liri_decoration_manager_destroy(Resource *resource) override;
};

class LiriDecoration : public QWaylandCompositorExtensionTemplate<LiriDecoration>
        , public QtWaylandServer::liri_decoration
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface *surface READ surface CONSTANT)
    Q_PROPERTY(QColor foregroundColor READ foregroundColor NOTIFY foregroundColorChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor NOTIFY backgroundColorChanged)
public:
    explicit LiriDecoration(LiriDecorationManager *parent, QWaylandSurface *surface);
    ~LiriDecoration();

    QWaylandSurface *surface() const;

    QColor foregroundColor() const;
    QColor backgroundColor() const;

Q_SIGNALS:
    void foregroundColorChanged(const QColor &color);
    void backgroundColorChanged(const QColor &color);

private:
    LiriDecorationManager *m_manager = nullptr;
    QPointer<QWaylandSurface> m_surface = nullptr;
    QColor m_fgColor = Qt::transparent;
    QColor m_bgColor = Qt::transparent;

    void liri_decoration_destroy_resource(Resource *resource) override;
    void liri_decoration_set_foreground(Resource *resource, const QString &colorName) override;
    void liri_decoration_set_background(Resource *resource, const QString &colorName) override;
    void liri_decoration_destroy(Resource *resource) override;
};

#endif // LIRIDECORATION_H
