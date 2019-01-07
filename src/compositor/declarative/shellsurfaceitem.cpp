/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "shellsurfaceitem.h"

ShellSurfaceItem::ShellSurfaceItem(QQuickItem *parent)
    : QWaylandQuickShellSurfaceItem(parent)
{
}

void ShellSurfaceItem::keyPressEvent(QKeyEvent *event)
{
    if (inputEventsEnabled()) {
        if (event->key() == Qt::Key_Alt)
            moveState.modifier = true;
    }

    QWaylandQuickShellSurfaceItem::keyPressEvent(event);
}

void ShellSurfaceItem::keyReleaseEvent(QKeyEvent *event)
{
    if (inputEventsEnabled()) {
        if (event->key() == Qt::Key_Alt) {
            moveState.modifier = false;
            moveState.initialized = false;

            if (moveState.started) {
                moveState.started = false;
                Q_EMIT moveStopped();
            }
        }
    }

    QWaylandQuickShellSurfaceItem::keyReleaseEvent(event);
}

void ShellSurfaceItem::mousePressEvent(QMouseEvent *event)
{
    if (inputEventsEnabled()) {
        if (event->buttons().testFlag(Qt::LeftButton))
            moveState.button = true;
    }

    QWaylandQuickShellSurfaceItem::mousePressEvent(event);
}

void ShellSurfaceItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        moveState.button = false;
        moveState.initialized = false;

        if (moveState.started) {
            moveState.started = false;
            Q_EMIT moveStopped();
        }
    }

    QWaylandQuickShellSurfaceItem::mouseReleaseEvent(event);
}

void ShellSurfaceItem::mouseMoveEvent(QMouseEvent *event)
{
    if (inputEventsEnabled()) {
        // Move as long as Alt+LMB were pressed
        if (moveState.modifier && moveState.button) {
            if (!moveState.initialized) {
                moveState.initialOffset = moveItem()->mapFromItem(nullptr, event->windowPos());
                moveState.initialized = true;
                moveState.started = true;
                Q_EMIT moveStarted();
            } else if (moveItem()->parentItem()) {
                QPointF parentPos = moveItem()->parentItem()->mapFromItem(nullptr, event->windowPos());
                moveItem()->setPosition(parentPos - moveState.initialOffset);
            }

            event->ignore();
            return;
        } else {
            moveState.initialized = false;
        }
    }

    QWaylandQuickShellSurfaceItem::mouseMoveEvent(event);
}
