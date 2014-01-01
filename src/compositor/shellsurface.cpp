/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "shellsurface.h"

ShellSurface::ShellSurface(Compositor::ShellWindowRole role, QWaylandSurface *surface)
    : QtWaylandServer::wl_hawaii_shell_surface()
    , m_role(role)
    , m_surface(surface)
    , m_popupGrabber(nullptr)
{
}

ShellSurface::~ShellSurface()
{
    if (m_popupGrabber)
        m_popupGrabber->setShellSurface(nullptr);
}

Compositor::ShellWindowRole ShellSurface::role() const
{
    return m_role;
}

QWaylandSurface *ShellSurface::surface() const
{
    return m_surface;
}

PopupGrabber *ShellSurface::popupGrabber() const
{
    return m_popupGrabber;
}

void ShellSurface::setPopupGrabber(PopupGrabber *grabber)
{
    m_popupGrabber = grabber;
    m_popupGrabber->setShellSurface(this);
}

void ShellSurface::hawaii_shell_surface_dismiss(Resource *resource)
{
    PopupGrabber *grabber = static_cast<PopupGrabber *>(
                wl_resource_get_user_data(resource->handle));
    if (grabber) {
        wl_resource_set_user_data(resource->handle, nullptr);
        grabber->setShellSurface(nullptr);
        delete grabber;
    }
}
