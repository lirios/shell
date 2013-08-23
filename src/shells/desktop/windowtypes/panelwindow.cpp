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
#include <QtQuick/QQuickItem>

#include <qpa/qplatformnativeinterface.h>

#include "panelwindow.h"
#include "waylandintegration.h"
#include "shellui.h"

PanelWindow::PanelWindow(ShellUi *ui)
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
    setSource(QUrl("qrc:/qml/Panel.qml"));

    // Resizing the view resizes the root object
    setResizeMode(QQuickView::SizeRootObjectToView);
    resetGeometry();

    // React to screen size changes
    connect(ui->screen(), SIGNAL(geometryChanged(QRect)),
            this, SLOT(geometryChanged(QRect)));
    connect(ui, SIGNAL(availableGeometryChanged(QRect)),
            this, SLOT(availableGeometryChanged(QRect)));

    // Debugging message
    qDebug() << "-> Created Panel with geometry"
             << geometry();
}

wl_surface *PanelWindow::surface() const
{
    return m_surface;
}

void PanelWindow::sendGeometry()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    hawaii_desktop_shell_set_panel_geometry(object->shell, output,
                                            m_surface,
                                            geometry().x(),
                                            geometry().y(),
                                            geometry().width(),
                                            geometry().height());
}

void PanelWindow::geometryChanged(const QRect &rect)
{
    qDebug() << "Resizing Panel because screen"
             << screen()->name() << "is now" << rect;

    // Resize and reposition the Panel
    resetGeometry();
}

void PanelWindow::availableGeometryChanged(const QRect &rect)
{
    rootObject()->setProperty("availableGeometry", rect);
}

void PanelWindow::resetGeometry()
{
    QRect rect = screen()->geometry();
    int size = rootObject()->property("size").toInt();

    setGeometry(QRect(rect.left(), rect.top(), rect.width(), size));
}

void PanelWindow::setWindowType()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    m_surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    hawaii_desktop_shell_set_panel(object->shell, output, m_surface);
}

#include "moc_panelwindow.cpp"
