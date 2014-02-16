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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "desktopview.h"
#include "shellmanager.h"

using namespace Hawaii::Shell;

DesktopView::DesktopView(ShellUi *corona, QScreen *screen)
    : QuickView(corona, new QWindow(screen))
{
    // Resize view to actual size and thus resize the root object
    setResizeMode(QQuickView::SizeRootObjectToView);

    // Set Wayland window type
    setWindowType();

    // Set geometry and react to screen geometry changes
    setGeometry(screen->geometry());
    connect(screen, &QScreen::geometryChanged,
            this, static_cast<void (QWindow::*)(const QRect &)>(&QWindow::setGeometry));

    // Debugging message
    qDebug() << "-> Created DesktopView with geometry"
             << geometry();
}

void DesktopView::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_background(output, surface);
}

#include "moc_desktopview.cpp"
