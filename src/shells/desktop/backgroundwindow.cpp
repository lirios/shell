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

#include "backgroundwindow.h"
#include "waylandintegration.h"
#include "shellui.h"

BackgroundWindow::BackgroundWindow(ShellUi *ui)
    : QQuickView(ui->engine(), new QWindow(ui->screen()))
    , m_surface(0)
{
    // Set custom window type
    setFlags(flags() | Qt::BypassWindowManagerHint);

    // Set Wayland window type
    create();
    setWindowType();

    // Load QML component
    setSource(QUrl("qrc:/qml/Background.qml"));

    // Resize view to actual size and thus resize the root object
    setResizeMode(QQuickView::SizeRootObjectToView);
    setGeometry(ui->screen()->geometry());

    // React to screen size changes
    connect(ui->screen(), SIGNAL(geometryChanged(QRect)),
            this, SLOT(geometryChanged(QRect)));

    // Debugging message
    qDebug() << "-> Created Background with geometry"
             << geometry();
}

wl_surface *BackgroundWindow::surface() const
{
    return m_surface;
}

void BackgroundWindow::geometryChanged(const QRect &rect)
{
    // Resize view to actual size
    setGeometry(rect);
}

void BackgroundWindow::setWindowType()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    m_surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    hawaii_desktop_shell_set_background(object->shell, output, m_surface);
}

#include "moc_backgroundwindow.cpp"
