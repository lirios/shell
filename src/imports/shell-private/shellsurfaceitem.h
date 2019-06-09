/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLSURFACEITEM_H
#define SHELLSURFACEITEM_H

#include <QWaylandQuickShellSurfaceItem>

class ShellSurfaceItem : public QWaylandQuickShellSurfaceItem
{
    Q_OBJECT
public:
    explicit ShellSurfaceItem(QQuickItem *parent = nullptr);

Q_SIGNALS:
    void moveStarted();
    void moveStopped();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    struct {
        bool modifier = false;
        bool button = false;
        bool initialized = false;
        bool started = false;
        QPointF initialOffset;
    } moveState;
};

#endif // SHELLSURFACEITEM_H
