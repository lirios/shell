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

#ifndef CHROMEITEM_H
#define CHROMEITEM_H

#include <QQuickItem>
#include <QWaylandCompositor>
#include <QWaylandQuickItem>

class ChromeItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QWaylandCompositor *compositor READ compositor WRITE setCompositor NOTIFY compositorChanged)
    Q_PROPERTY(QWaylandQuickItem *shellSurfaceItem READ shellSurfaceItem WRITE setShellSurfaceItem NOTIFY shellSurfaceItemChanged)
    Q_PROPERTY(bool primary READ isPrimary NOTIFY primaryChanged)
public:
    ChromeItem(QQuickItem *parent = nullptr);

    QWaylandCompositor *compositor() const;
    void setCompositor(QWaylandCompositor *compositor);

    QWaylandQuickItem *shellSurfaceItem() const;
    void setShellSurfaceItem(QWaylandQuickItem *item);

    bool isPrimary() const;

    Q_INVOKABLE QPointF randomPosition(const QPointF &mousePos) const;

Q_SIGNALS:
    void compositorChanged();
    void shellSurfaceItemChanged();
    void primaryChanged();

public Q_SLOTS:
    void raise();
    void lower();
    void takeFocus(QWaylandSeat *device = nullptr);

private:
    QWaylandCompositor *m_compositor;
    QWaylandQuickItem *m_shellSurfaceItem = nullptr;
    bool m_primary = false;

private Q_SLOTS:
    void updatePrimary();
};

#endif // CHROMEITEM_H
