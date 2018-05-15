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

#pragma once

#include <QtCore/QElapsedTimer>
#include <QtQuick/QQuickItem>

class HotSpot : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Qt::Corner corner READ corner WRITE setCorner NOTIFY cornerChanged)
    Q_PROPERTY(quint64 threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
    Q_PROPERTY(quint64 pushTime READ pushTime WRITE setPushTime NOTIFY pushTimeChanged)
    Q_PROPERTY(bool hovered READ hovered NOTIFY hoveredChanged)
public:
    explicit HotSpot(QQuickItem *parent = nullptr);

    Qt::Corner corner() const;
    void setCorner(Qt::Corner corner);

    quint64 threshold() const;
    void setThreshold(quint64 threshold);

    quint64 pushTime() const;
    void setPushTime(quint64 time);

    bool hovered() const;

Q_SIGNALS:
    void cornerChanged();
    void thresholdChanged();
    void pushTimeChanged();
    void hoveredChanged();
    void triggered();

protected:
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    Qt::Corner m_corner;
    quint64 m_threshold;
    quint64 m_pushTime;
    quint64 m_lastTime;
    quint64 m_entered;
    QElapsedTimer m_timer;
    bool m_hovered;
};
