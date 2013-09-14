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

#include "qwayland-desktop.h"

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

class Window;

class WindowPrivate : public QtWayland::hawaii_window
{
    Q_DECLARE_PUBLIC(Window)
public:
    WindowPrivate();
    ~WindowPrivate();

    Window *q_ptr;
    QString title;
    QString identifier;
    Window::States state;
    AppInfo *appInfo;

protected:
    virtual void hawaii_window_title_changed(const QString &title);
    virtual void hawaii_window_identifier_changed(const QString &identifier);
    virtual void hawaii_window_state_changed(int32_t state);
    virtual void hawaii_window_unmapped();
};

#endif // WINDOW_P_H
