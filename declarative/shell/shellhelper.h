/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2016 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <GreenIsland/QtWaylandCompositor/QWaylandCompositorExtension>

class QWaylandSurface;

class ShellHelperPrivate;

class ShellHelper : public QWaylandCompositorExtensionTemplate<ShellHelper>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ShellHelper)
public:
    enum GrabCursor {
        NoGrabCursor = 0,
        ResizeTopGrabCursor,
        ResizeBottomGrabCursor,
        ArrowGrabCursor,
        ResizeLeftGrabCursor,
        ResizeTopLeftGrabCursor,
        ResizeBottomLeftGrabCursor,
        MoveGrabCursor,
        ResizeRightGrabCursor,
        ResizeTopRightGrabCursor,
        ResizeBottomRightGrabCursor,
        BusyGrabCursor
    };
    Q_ENUM(GrabCursor)

    ShellHelper();
    explicit ShellHelper(QWaylandCompositor *compositor);

    void initialize() Q_DECL_OVERRIDE;

    Q_INVOKABLE void start(const QString &socketName);

    Q_INVOKABLE void grabCursor(GrabCursor cursor);

    static const struct wl_interface *interface();
    static QByteArray interfaceName();

Q_SIGNALS:
    void grabSurfaceAdded(QWaylandSurface *surface);
};
