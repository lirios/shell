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
#include "hawaiishell.h"
#include "hawaiishell_p.h"

using namespace Hawaii::Shell;

DesktopView::DesktopView(QQmlEngine *engine, QScreen *screen)
    : QuickView(engine, new QWindow(screen))
{
    // Set Wayland window type
    setWindowType();

    // React to screen geometry changes
    connect(this, &QuickView::screenGeometryChanged,
            this, &DesktopView::changeGeometry);

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

    HawaiiShell::instance()->d_ptr->shell->set_background(output, surface);
}

void DesktopView::changeGeometry(const QRect &geometry)
{
    setGeometry(geometry);
}

#include "moc_desktopview.cpp"
