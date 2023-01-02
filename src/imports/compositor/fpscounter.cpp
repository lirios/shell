// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
