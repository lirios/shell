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

#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <QElapsedTimer>
#include <QPointer>
#include <QQuickItem>
#include <QQuickWindow>

class FpsCounter : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal framesPerSecond READ framesPerSecond NOTIFY framesPerSecondChanged)
public:
    explicit FpsCounter(QQuickItem *parent = nullptr);

    qreal framesPerSecond() const;

Q_SIGNALS:
    void framesPerSecondChanged();

protected:
    void timerEvent(QTimerEvent *) override;

private:
    QPointer<QQuickWindow> m_lastWindow;
    QMetaObject::Connection m_lastWindowConnection;
    QElapsedTimer m_timer;
    int m_counter = 0;
    qreal m_fps = 0;

    void resetFramesPerSecond();

private Q_SLOTS:
    void handleWindowChanged(QQuickWindow *window);
};

#endif // FPSCOUNTER_H
