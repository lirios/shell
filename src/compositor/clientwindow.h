/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QtCompositor/QWaylandSurface>

#include "qwayland-server-hawaii.h"

class ClientWindow : public QtWaylandServer::wl_hawaii_window
{
public:
    ClientWindow(struct ::wl_display *display);

    QWaylandSurface *surface() const;
    void setSurface(QWaylandSurface *surface);

    int32_t state() const;
    void setState(int32_t newState);

    bool isMapped() const;

    bool isActive() const;
    void activate();
    void deactivate();

    void minimize();
    void unminimize();

protected:
    void hawaii_window_set_state(Resource *resource, int32_t newState);

private:
    bool m_mapped;
    QWaylandSurface *m_surface;
    int32_t m_state;
};

#endif // CLIENTWINDOW_H
