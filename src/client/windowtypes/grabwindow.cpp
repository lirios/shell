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

#include <QtGui/QGuiApplication>

#include <qpa/qplatformnativeinterface.h>

#include "grabwindow.h"
#include "shellmanager.h"

GrabWindow::GrabWindow()
    : QWindow()
{
    // Set window type and resize
    setFlags(flags() | Qt::BypassWindowManagerHint);
    resize(1, 1);
    create();
    setWindowType();
}

void GrabWindow::setGrabCursor(const QCursor &cursor)
{
    // This is needed because QWindow::setCursor() is not invokable
    setCursor(cursor);
}

void GrabWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_grab_surface(surface);
}

#include "moc_grabwindow.cpp"
