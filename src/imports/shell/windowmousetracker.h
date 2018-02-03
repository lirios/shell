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

#pragma once

#include <QtQuick/QQuickItem>

class WindowMouseTrackerPrivate;

class WindowMouseTracker : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mouseXChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mouseYChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(bool windowSystemCursorEnabled READ windowSystemCursorEnabled WRITE setWindowSystemCursorEnabled NOTIFY windowSystemCursorEnabledChanged)
    Q_DECLARE_PRIVATE(WindowMouseTracker)
public:
    explicit WindowMouseTracker(QQuickItem *parent = nullptr);
    ~WindowMouseTracker();

    qreal mouseX() const;
    qreal mouseY() const;

    bool containsMouse() const;

    bool windowSystemCursorEnabled() const;
    void setWindowSystemCursorEnabled(bool enable);

Q_SIGNALS:
    void mouseXChanged();
    void mouseYChanged();
    void containsMouseChanged();
    void windowSystemCursorEnabledChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;

private:
    WindowMouseTrackerPrivate *const d_ptr;
};
