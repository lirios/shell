/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
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

#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include "interface.h"
#include "wayland-notification-daemon-server-protocol.h"

class DesktopShell;

class Notifications : public Interface
{
public:
    enum Edge {
        EdgeTop = 1 << 0,
        EdgeBottom = 1 << 1,
        EdgeLeft = 1 << 2,
        EdgeRight = 1 << 3
    };

    enum CornerAnchors {
        CornerTopRight = EdgeTop | EdgeRight,
        CornerTopLeft = EdgeTop | EdgeLeft,
        CornerBottomRight = EdgeBottom | EdgeRight,
        CornerBottomLeft = EdgeBottom | EdgeLeft
    };

    enum NotificationsOrder {
        OrderNewestFirst = 0,
        OrderOldestFirst
    };

    Notifications();

private:
    friend class DesktopShell;

    void bind(wl_client *client, uint32_t version, uint32_t id);
    void unbind(wl_resource *resource);

    void mapSurfaces();
    void addSurface(wl_client *client,
                    wl_resource *resource,
                    wl_resource *surface_resource);

    wl_resource *m_binding;

    Layer m_notificationsLayer;
    Edge m_edge;
    CornerAnchors m_cornerAnchor;
    NotificationsOrder m_order;
    int32_t m_margin;

    static const struct wl_notification_daemon_interface implementation;
};

#endif // NOTIFICATIONS_H
