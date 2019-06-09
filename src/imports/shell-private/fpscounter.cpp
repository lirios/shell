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

#include "fpscounter.h"

FpsCounter::FpsCounter(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(QQuickItem::ItemHasContents, false);

    handleWindowChanged(window());
    connect(this, &QQuickItem::windowChanged, this, &FpsCounter::handleWindowChanged);

    startTimer(1000);
    m_timer.start();
}

qreal FpsCounter::framesPerSecond() const
{
    return m_fps;
}

void FpsCounter::timerEvent(QTimerEvent *)
{
    resetFramesPerSecond();
    m_counter = 0;
    m_timer.start();
}

void FpsCounter::resetFramesPerSecond()
{
    m_fps = m_counter * 1000.0 / m_timer.elapsed();
    Q_EMIT framesPerSecondChanged();
}

void FpsCounter::handleWindowChanged(QQuickWindow *window)
{
    if (!m_lastWindow.isNull())
        disconnect(m_lastWindowConnection);

    if (!window)
        return;

    m_lastWindow = window;
    m_lastWindowConnection = connect(window, &QQuickWindow::frameSwapped, this, [this] {
        m_counter++;
    });
}
