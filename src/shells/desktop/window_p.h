/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef WINDOW_P_H
#define WINDOW_P_H

#include "window.h"
#include "wayland-desktop-extension-client-protocol.h"

struct hawaii_window;
struct hawaii_window_listener;

/*!
    Converts a window state integer provided by the compositor
    into a Window::State value that the shell can understand.
*/
static Window::States wlStateConvert(int32_t state)
{
    Window::States result = Window::Inactive;

    if (state & HAWAII_DESKTOP_SHELL_WINDOW_STATE_ACTIVE)
        result |= Window::Active;
    if (state & HAWAII_DESKTOP_SHELL_WINDOW_STATE_MINIMIZED)
        result |= Window::Minimized;
    if (state & HAWAII_DESKTOP_SHELL_WINDOW_STATE_MAXIMIZED)
        result |= Window::Maximized;
    if (state & HAWAII_DESKTOP_SHELL_WINDOW_STATE_FULLSCREEN)
        result |= Window::Fullscreen;

    return result;
}

/*!
    Converts a window state provided by the shell to an
    integer that the compositor can understand.
*/
static int32_t stateConvert(Window::States state)
{
    int32_t result = HAWAII_DESKTOP_SHELL_WINDOW_STATE_INACTIVE;

    if (state & Window::Active)
        result |= HAWAII_DESKTOP_SHELL_WINDOW_STATE_ACTIVE;
    if (state & Window::Minimized)
        result |= HAWAII_DESKTOP_SHELL_WINDOW_STATE_MINIMIZED;
    if (state & Window::Maximized)
        result |= HAWAII_DESKTOP_SHELL_WINDOW_STATE_MAXIMIZED;
    if (state & Window::Fullscreen)
        result |= HAWAII_DESKTOP_SHELL_WINDOW_STATE_FULLSCREEN;

    return result;
}

class WindowPrivate
{
    Q_DECLARE_PUBLIC(Window)
public:
    WindowPrivate();

    void initialize(hawaii_window *window);

    Window *q_ptr;
    hawaii_window *window;
    QString title;
    QString identifier;
    Window::States state;

private:
    void handleTitleChanged(hawaii_window *window, const char *title);
    void handleIdentifierChanged(hawaii_window *window, const char *id);
    void handleStateChanged(hawaii_window *window, int32_t state);
    void handleUnmapped(hawaii_window *window);

    static const hawaii_window_listener listener;
};

#endif // WINDOW_P_H
