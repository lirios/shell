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

#include <QtWaylandCompositor/QWaylandOutput>

#include "chromeitem.h"

ChromeItem::ChromeItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_compositor(nullptr)
{
    connect(this, &ChromeItem::xChanged, this, &ChromeItem::updatePrimary);
    connect(this, &ChromeItem::yChanged, this, &ChromeItem::updatePrimary);
}

QWaylandCompositor *ChromeItem::compositor() const
{
    return m_compositor;
}

void ChromeItem::setCompositor(QWaylandCompositor *compositor)
{
    if (m_compositor == compositor)
        return;

    m_compositor = compositor;
    Q_EMIT compositorChanged();
}

QWaylandQuickItem *ChromeItem::shellSurfaceItem() const
{
    return m_shellSurfaceItem;
}

void ChromeItem::setShellSurfaceItem(QWaylandQuickItem *item)
{
    if (m_shellSurfaceItem == item)
        return;

    m_shellSurfaceItem = item;
    Q_EMIT shellSurfaceItemChanged();
}

bool ChromeItem::isPrimary() const
{
    return m_primary;
}

QPointF ChromeItem::randomPosition(const QPointF &mousePos) const
{
    if (!m_compositor)
        return QPointF(0, 0);

    // Surface size
    const qreal w = width();
    const qreal h = height();

    // Find the output where the pointer is located, defaults
    // to the default output
    // TODO: Need something clever for touch?
    QWaylandOutput *output = m_compositor->defaultOutput();
    for (QWaylandOutput *curOutput : m_compositor->outputs()) {
        if (curOutput->geometry().contains(mousePos.toPoint())) {
            output = curOutput;
            break;
        }
    }

    // Output size
    const int ow = output->availableGeometry().size().width();
    const int oh = output->availableGeometry().size().height();

    const int step = 24;
    static int px = step;
    static int py = 2 * step;
    qreal dx, dy;

    // Increment new coordinates by the step
    px += step;
    py += 2 * step;
    if (px > ow / 2)
        px = step;
    if (py > oh / 2)
        py = step;
    dx = px;
    dy = py;
    if (dx + w > ow) {
        dx = ow - w;
        if (dx < 0)
            dx = 0;
        px = 0;
    }
    if (dy + h > oh) {
        dy = oh - h;
        if (dy < 0)
            dy = 0;
        py = 0;
    }

    return QPointF(dx, dy);
}

void ChromeItem::lower()
{
    QQuickItem *parent = parentItem();
    Q_ASSERT(parent);
    QQuickItem *bottom = parent->childItems().first();
    if (bottom != this)
        stackBefore(bottom);
}

void ChromeItem::takeFocus(QWaylandSeat *device)
{
    if (m_shellSurfaceItem)
        m_shellSurfaceItem->takeFocus(device);
}

void ChromeItem::updatePrimary()
{
    if (!m_shellSurfaceItem)
        return;

    const qreal xOrig = x();
    const qreal yOrig = y();
    const qreal w = width();
    const qreal h = height();
    const qreal area = w * h;
    const qreal screenWidth = m_shellSurfaceItem->output()->geometry().width();
    const qreal screenHeight = m_shellSurfaceItem->output()->geometry().height();
    const qreal x1 = qMax<qreal>(0, xOrig);
    const qreal y1 = qMax<qreal>(0, yOrig);
    const qreal x2 = qMin<qreal>(xOrig + w, screenWidth);
    const qreal y2 = qMin<qreal>(yOrig + h, screenHeight);
    const qreal w1 = qMax<qreal>(0, x2 - x1);
    const qreal h1 = qMax<qreal>(0, y2 - y1);

    const bool oldPrimaryValue = m_primary;
    if (w1 * h1 * 2 > area) {
        m_shellSurfaceItem->setPrimary();
        m_primary = true;
    } else {
        m_primary = false;
    }
    if (oldPrimaryValue != m_primary)
        Q_EMIT primaryChanged();
}

void ChromeItem::raise()
{
    QQuickItem *parent = parentItem();
    Q_ASSERT(parent);
    QQuickItem *top = parent->childItems().last();
    if (top != this)
        stackAfter(top);
}
