/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/QWaylandInputDevice>

#include "clientwindow.h"
#include "compositor.h"
#include "shell.h"

ClientWindow::ClientWindow(struct ::wl_display *display)
    : QtWaylandServer::wl_hawaii_window(display)
    , m_mapped(false)
    , m_surface(nullptr)
    , m_state(WL_HAWAII_SHELL_WINDOW_STATE_INACTIVE)
{
}

QWaylandSurface *ClientWindow::surface() const
{
    return m_surface;
}

void ClientWindow::setSurface(QWaylandSurface *surface)
{
    if (m_surface == surface)
        return;

    m_surface = surface;
    if (!m_surface)
        return;

    QObject::connect(m_surface, &QWaylandSurface::titleChanged, [=]() {
        if (m_mapped) {
            for (Resource *resource: resourceMap().values())
                send_title_changed(resource->handle, m_surface->title());
        }
    });
    QObject::connect(m_surface, &QWaylandSurface::classNameChanged, [=]() {
        if (m_mapped) {
            for (Resource *resource: resourceMap().values())
                send_identifier_changed(resource->handle, m_surface->className());
        }
    });
    QObject::connect(m_surface, &QWaylandSurface::visibilityChanged, [=]() {
        if (!m_mapped)
            return;

        switch (m_surface->visibility()) {
        case QWindow::Minimized:
            m_state = WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED;
            break;
        case QWindow::Maximized:
            m_state = WL_HAWAII_SHELL_WINDOW_STATE_MAXIMIZED;
            break;
        case QWindow::FullScreen:
            m_state = WL_HAWAII_SHELL_WINDOW_STATE_FULLSCREEN;
            break;
        default:
            break;
        }

        for (Resource *resource: resourceMap().values())
            send_state_changed(resource->handle, m_state);
    });
    QObject::connect(m_surface, &QWaylandSurface::mapped, [=]() {
        if (m_mapped)
            return;

        for (Resource *resource: resourceMap().values())
            Compositor::instance()->shell()->send_window_mapped(
                        resource->handle,
                        m_surface->title(),
                        m_surface->className(),
                        m_state);
        m_mapped = true;

        if (m_surface->surfaceItem()) {
            QObject::connect(m_surface->surfaceItem(), &QWaylandSurfaceItem::focusChanged, [=](bool focus) {
                if (focus)
                    m_state |= WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE;
                else
                    m_state &= ~WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE;
            });

            for (Resource *resource: resourceMap().values())
                send_state_changed(resource->handle, m_state);
        }
    });
    QObject::connect(m_surface, &QWaylandSurface::unmapped, [=]() {
        if (m_mapped) {
            for (Resource *resource: resourceMap().values())
                send_unmapped(resource->handle);
            m_mapped = false;
        }
    });
}

int32_t ClientWindow::state() const
{
    return m_state;
}

void ClientWindow::setState(int32_t newState)
{
    if (m_state != newState) {
        m_state = newState;
        for (Resource *resource: resourceMap().values())
            send_state_changed(resource->handle, m_state);
    }
}

bool ClientWindow::isMapped() const
{
    return m_mapped;
}

bool ClientWindow::isActive() const
{
    return m_state & WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE;
}

void ClientWindow::activate()
{
    if (m_surface) {
        if (m_surface->surfaceItem())
            m_surface->surfaceItem()->takeFocus();
        setState(m_state | WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE);
    }
}

void ClientWindow::deactivate()
{
    if (m_surface) {
        if (m_surface->surfaceItem()) {
            m_surface->surfaceItem()->setFocus(false);
            m_surface->compositor()->defaultInputDevice()->setKeyboardFocus(0);
        }
        setState(m_state & ~WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE);
    }
}

void ClientWindow::minimize()
{
    if (m_surface) {
        m_surface->setVisibility(QWindow::Hidden);
        setState(m_state & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED);
    }
}

void ClientWindow::unminimize()
{
    if (m_surface) {
        m_surface->setVisibility(QWindow::AutomaticVisibility);
        setState(m_state & ~WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED);
    }
}

void ClientWindow::hawaii_window_set_state(Resource *resource, int32_t newState)
{
    ClientWindow *window = static_cast<ClientWindow *>(resource->hawaii_window);
    int32_t state = window->state();

    if (state & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED && !(newState & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED)) {
        window->unminimize();
    } else if (newState & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED && !(state & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED)) {
        window->minimize();

        if (window->isActive())
            window->deactivate();
    }

    if (newState & WL_HAWAII_SHELL_WINDOW_STATE_ACTIVE && !(newState & WL_HAWAII_SHELL_WINDOW_STATE_MINIMIZED))
        window->activate();
}
