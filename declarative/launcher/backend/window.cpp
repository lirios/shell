/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "window.h"

Window::Window(struct ::greenisland_window *object, Type type, QObject *parent)
    : QObject(parent)
    , QtWayland::greenisland_window(object)
    , m_type(type)
    , m_workspace(0)
{
}

Window::~Window()
{
    greenisland_window_destroy(object());
}

Window::Type Window::type() const
{
    return m_type;
}

QString Window::title() const
{
    return m_title;
}

QString Window::appId() const
{
    return m_appId;
}

quint32 Window::workspace() const
{
    return m_workspace;
}

bool Window::isActive() const
{
    return m_state & QtWayland::greenisland_windows::state_active;
}

bool Window::isMinimized() const
{
    return m_state & QtWayland::greenisland_windows::state_minimized;
}

bool Window::isMaximized() const
{
    return m_state & QtWayland::greenisland_windows::state_maximized;
}

bool Window::isFullScreen() const
{
    return m_state & QtWayland::greenisland_windows::state_fullscreen;
}

void Window::window_title_changed(const QString &title)
{
    m_title = title;
    Q_EMIT titleChanged();
}

void Window::window_app_id_changed(const QString &appId)
{
    m_appId = appId;
    Q_EMIT appIdChanged();
}

void Window::window_state_changed(uint32_t flags)
{
    m_state = flags;
    Q_EMIT stateChanged();
}

void Window::window_workspace_changed(uint32_t number)
{
    m_workspace = number;
    Q_EMIT workspaceChanged();
}

void Window::window_unmapped()
{
    Q_EMIT unmapped();
}

#include "moc_window.cpp"
