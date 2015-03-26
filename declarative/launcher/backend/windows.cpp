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

#include "applicationmanager.h"
#include "window.h"
#include "windows.h"

Windows::Windows(ApplicationManager *parent)
    : QObject(parent)
    , m_parent(parent)
{
}

Windows::~Windows()
{
    while (!m_windows.isEmpty())
        m_windows.takeFirst()->deleteLater();

    greenisland_windows_destroy(object());
}

QList<Window *> Windows::windows() const
{
    return m_windows;
}

void Windows::windows_window_mapped(greenisland_window *id,
                                    uint32_t type, uint32_t state,
                                    const QString &title, const QString &appId)
{
    Window::Type windowType = Window::TopLevel;
    switch (type) {
    case QtWayland::greenisland_windows::type_popup:
        windowType = Window::Popup;
        break;
    case QtWayland::greenisland_windows::type_transient:
        windowType = Window::Transient;
        break;
    default:
        break;
    }

    Window *window = new Window(id, windowType, this);
    window->m_state = state;
    window->m_title = title;
    window->m_appId = appId;
    m_windows.append(window);

    connect(window, &Window::unmapped, this, [this, window]() {
        m_windows.removeOne(window);
        window->deleteLater();
    });
}

#include "moc_windows.cpp"
