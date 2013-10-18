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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>

#include <qpa/qplatformnativeinterface.h>

#include "overlaywindow.h"
#include "desktopshell.h"
#include "desktopshell_p.h"
#include "shellui.h"

OverlayWindow::OverlayWindow(ShellUi *ui)
    : QQuickView(ui->engine(), new QWindow(ui->screen()))
    , m_surface(0)
{
    // Set transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(flags() | Qt::BypassWindowManagerHint);

    // Set Wayland window type
    create();
    setWindowType();

    // Load QML component
    setSource(QUrl("qrc:/qml/Overlay.qml"));

    // Resize view to actual size and thus resize the root object
    setResizeMode(QQuickView::SizeRootObjectToView);
    geometryChanged(ui->screen()->geometry());

    // React to screen size changes
    connect(ui->screen(), SIGNAL(geometryChanged(QRect)),
            this, SLOT(geometryChanged(QRect)));

    // Debugging message
    qDebug() << "-> Created Overlay with geometry"
             << geometry();
}

wl_surface *OverlayWindow::surface() const
{
    return m_surface;
}

void OverlayWindow::geometryChanged(const QRect &rect)
{
    // Resize view to actual size
    setGeometry(rect);

    // Set surface position
    setSurfacePosition(position());
}

void OverlayWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    DesktopShellImpl *shell = DesktopShell::instance()->d_ptr->shell;
    shell->set_overlay(output, surface);
}

void OverlayWindow::setSurfacePosition(const QPoint &pt)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    DesktopShellImpl *shell = DesktopShell::instance()->d_ptr->shell;
    shell->set_position(surface, pt.x(), pt.y());
}

#include "moc_overlaywindow.cpp"
