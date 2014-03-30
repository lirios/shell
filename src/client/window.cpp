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

#include <QtCore/QStandardPaths>
#include <QtQml/QQmlEngine>

#include "window.h"
#include "window_p.h"
#include "shellmanager.h"

using namespace Hawaii;

/*
 * WindowPrivate
 */

WindowPrivate::WindowPrivate()
    : QtWayland::wl_hawaii_window()
    , q_ptr(0)
    , state(Window::Inactive)
    , appInfo(0)
{
}

WindowPrivate::~WindowPrivate()
{
    delete appInfo;
    wl_hawaii_window_destroy(object());
}

void WindowPrivate::hawaii_window_title_changed(const QString &title)
{
    Q_Q(Window);

    if (this->title != title) {
        this->title = title;
        Q_EMIT q->titleChanged(this->title);
    }
}

void WindowPrivate::hawaii_window_identifier_changed(const QString &identifier)
{
    Q_Q(Window);

    if (this->identifier != identifier) {
        this->identifier = identifier;
        Q_EMIT q->identifierChanged(this->identifier);

        QString fileName = QStandardPaths::locate(
                    QStandardPaths::ApplicationsLocation,
                    identifier);
        delete this->appInfo;
        this->appInfo = new Hawaii::AppInfo();
        this->appInfo->moveToThread(ShellManager::instance()->mantle()->engine()->thread());
        this->appInfo->load(fileName);
        Q_EMIT q->appInfoChanged();
    }
}

void WindowPrivate::hawaii_window_state_changed(int32_t state)
{
    Q_Q(Window);

    if (this->state != wlStateConvert(state)) {
        bool wasActive = this->state & Window::Active;
        bool isActive = state & Window::Active;
        bool wasMinimized = this->state & Window::Minimized;
        bool isMinimized = state & Window::Minimized;
        bool wasMaximized = this->state & Window::Maximized;
        bool isMaximized = state & Window::Maximized;
        bool wasFullscreen = this->state & Window::Fullscreen;
        bool isFullscreen = state & Window::Fullscreen;

        this->state = wlStateConvert(state);
        Q_EMIT q->stateChanged(this->state);

        if (wasActive != isActive)
            Q_EMIT q->activeChanged(isActive);
        if (wasMinimized != isMinimized)
            Q_EMIT q->minimizedChanged(isMinimized);
        if (wasMaximized != isMaximized)
            Q_EMIT q->maximizedChanged(isMaximized);
        if (wasFullscreen != isFullscreen)
            Q_EMIT q->fullscreenChanged(isFullscreen);
    }
}

void WindowPrivate::hawaii_window_unmapped()
{
    Q_Q(Window);

    Q_EMIT q->unmapped(q);
    q->deleteLater();
}

/*
 * Window
 */

Window::Window(const QString &title, const QString &identifier, States state, QObject *parent)
    : QObject(parent)
    , d_ptr(new WindowPrivate())
{
    Q_D(Window);
    d->q_ptr = this;
    d->title = title;
    d->identifier = identifier;
    d->state = state;

    QString fileName = QStandardPaths::locate(
                QStandardPaths::ApplicationsLocation,
                identifier);
    d->appInfo = new Hawaii::AppInfo();
    d->appInfo->moveToThread(ShellManager::instance()->mantle()->engine()->thread());
    d->appInfo->load(fileName);
}

Window::Window(QObject *parent)
    : QObject(parent)
    , d_ptr(new WindowPrivate())
{
    Q_D(Window);
    d->q_ptr = this;
}

Window::~Window()
{
    delete d_ptr;
}

QString Window::title() const
{
    Q_D(const Window);
    return d->title;
}

QString Window::identifier() const
{
    Q_D(const Window);
    return d->identifier;
}

bool Window::isActive() const
{
    Q_D(const Window);
    return d->state & Window::Active;
}

bool Window::isMinimized() const
{
    Q_D(const Window);
    return d->state & Window::Minimized;
}

bool Window::isMaximized() const
{
    Q_D(const Window);
    return d->state & Window::Maximized;
}

bool Window::isFullscreen() const
{
    Q_D(const Window);
    return d->state & Window::Fullscreen;
}

Window::States Window::state() const
{
    Q_D(const Window);
    return d->state;
}

void Window::setState(const States &state)
{
    Q_D(Window);

    if (state != d->state) {
        d->state = state;
        d->set_state(stateConvert(d->state));
    }
}

Hawaii::AppInfo *Window::appInfo() const
{
    Q_D(const Window);
    return d->appInfo;
}

void Window::activate()
{
    Q_D(Window);

    if (!isActive())
        setState(d->state | Window::Active);
}

void Window::deactivate()
{
    Q_D(Window);

    if (isActive())
        setState(d->state & ~Window::Active);
}

void Window::minimize()
{
    Q_D(Window);

    if (!(d->state & Window::Minimized))
        setState(d->state | Window::Minimized);
}

void Window::unminimize()
{
    Q_D(Window);

    if (d->state & Window::Minimized)
        setState(d->state & ~Window::Minimized);
}

void Window::maximize()
{
    Q_D(Window);

    if (!(d->state & Window::Maximized))
        setState(d->state | Window::Maximized);
}

void Window::restore()
{
    Q_D(Window);

    if (d->state & Window::Maximized)
        setState(d->state & ~Window::Maximized);
}

#include "moc_window.cpp"
