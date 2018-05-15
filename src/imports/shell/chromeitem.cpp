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
    , m_isModifierHeld(false)
{
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

QPointF ChromeItem::randomPosition(const QPointF &mousePos, const QSizeF &surfaceSize) const
{
    if (!m_compositor)
        return QPointF(0, 0);

    // Surface size
    const int w = surfaceSize.toSize().width();
    const int h = surfaceSize.toSize().height();

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
    int dx, dy;

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

void ChromeItem::keyPressEvent(QKeyEvent *event)
{
    m_isModifierHeld = event->key() == Qt::Key_Meta ||
            event->key() == Qt::Key_Super_L ||
            event->key() == Qt::Key_Super_R;

    QQuickItem::keyPressEvent(event);
}

void ChromeItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Meta || event->key() == Qt::Key_Super_L ||
            event->key() == Qt::Key_Super_R)
        m_isModifierHeld = false;

    QQuickItem::keyReleaseEvent(event);
}

void ChromeItem::mousePressEvent(QMouseEvent *event)
{
    // Let mouse press go through anyway, if focus on click is enabled this
    // will give focus to the window before the use can drag it
    QQuickItem::mousePressEvent(event);

    // If the modifier is pressed we initiate a move operation
    if (m_isModifierHeld && event->buttons().testFlag(Qt::LeftButton))
        Q_EMIT moveRequested();
}

void ChromeItem::lower()
{
    QQuickItem *parent = parentItem();
    Q_ASSERT(parent);
    QQuickItem *bottom = parent->childItems().first();
    if (bottom != this)
        stackBefore(bottom);
}

void ChromeItem::raise()
{
    QQuickItem *parent = parentItem();
    Q_ASSERT(parent);
    QQuickItem *top = parent->childItems().last();
    if (top != this)
        stackAfter(top);
}
