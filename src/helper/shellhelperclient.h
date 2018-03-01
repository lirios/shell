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

#ifndef LIRISHELLHELPERCLIENT_H
#define LIRISHELLHELPERCLIENT_H

#include <QtCore/QObject>
#include <QtWaylandClient/qwaylandclientextension.h>

#include <wayland-client.h>

class QWindow;

class ShellHelperClientPrivate;

class ShellHelperClient : public QWaylandClientExtensionTemplate<ShellHelperClient>
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ShellHelperClient)
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

    ShellHelperClient();
    ~ShellHelperClient();

    void init(struct ::wl_registry *registry, int id, int version);

    Q_INVOKABLE void registerGrabSurface(QWindow *window);

    static const struct ::wl_interface *interface();

Q_SIGNALS:
    void cursorChangeRequested(GrabCursor cursor);

private:
    ShellHelperClientPrivate *const d_ptr;

    bool eventFilter(QObject *watched, QEvent *event) override;
    void handleActivation();
};

#endif // LIRISHELLHELPERCLIENT_H
