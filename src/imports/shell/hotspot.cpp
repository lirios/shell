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

#include "hotspot.h"

HotSpot::HotSpot(QQuickItem *parent)
    : QQuickItem(parent)
    , m_corner(Qt::TopLeftCorner)
    , m_threshold(1000)
    , m_pushTime(50)
    , m_lastTime(0)
    , m_entered(0)
    , m_hovered(false)
{
    setAcceptHoverEvents(true);
}

Qt::Corner HotSpot::corner() const
{
    return m_corner;
}

void HotSpot::setCorner(Qt::Corner corner)
{
    if (m_corner == corner)
        return;

    m_corner = corner;
    Q_EMIT cornerChanged();
}

quint64 HotSpot::threshold() const
{
    return m_threshold;
}

void HotSpot::setThreshold(quint64 threshold)
{
    if (m_threshold == threshold)
        return;

    m_threshold = threshold;
    Q_EMIT thresholdChanged();
}

quint64 HotSpot::pushTime() const
{
    return m_pushTime;
}

void HotSpot::setPushTime(quint64 time)
{
    if (m_pushTime == time)
        return;

    m_pushTime = time;
    Q_EMIT pushTimeChanged();
}

bool HotSpot::hovered() const
{
    return m_hovered;
}

void HotSpot::hoverEnterEvent(QHoverEvent *event)
{
    m_hovered = true;
    Q_EMIT hoveredChanged();
    event->setAccepted(true);
}

void HotSpot::hoverLeaveEvent(QHoverEvent *event)
{
    m_hovered = false;
    Q_EMIT hoveredChanged();
    event->setAccepted(true);
}

void HotSpot::hoverMoveEvent(QHoverEvent *event)
{
    // Hot spots are triggered after the threshold
    qint64 time = m_timer.elapsed();
    if (time - m_lastTime >= m_threshold) {
        const QPointF pos = event->posF();
        bool inside = false;

        if (pos.x() <= 5 && pos.y() >= 5)
            inside = true;
        else
            m_entered = 0;

        if (inside) {
            if (m_entered == 0) {
                m_entered = time;
            } else if (time - m_entered < m_pushTime) {
                m_lastTime = time;
                Q_EMIT triggered();
            }
        }
    }

    event->setAccepted(true);
}
