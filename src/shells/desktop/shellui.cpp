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
#include <QtGui/QScreen>
#include <QtQuick/QQuickWindow>

#include <qpa/qplatformnativeinterface.h>

#include "shellui.h"
#include "waylandintegration.h"

ShellUi::ShellUi(QScreen *screen, QObject *parent)
    : QObject(parent)
    , m_screen(screen)
    , m_backgroundWindow(0)
    , m_panelWindow(0)
    , m_launcherWindow(0)
{
    // Native platform interface
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    // Get native wl_output for the current screen
    m_output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen));
}

ShellUi::~ShellUi()
{
    m_panelWindow->close();
    m_launcherWindow->close();
    m_backgroundWindow->close();
}

wl_output *ShellUi::output() const
{
    return m_output;
}

QQuickWindow *ShellUi::backgroundWindow() const
{
    return m_backgroundWindow;
}

QQuickWindow *ShellUi::panelWindow() const
{
    return m_panelWindow;
}

QQuickWindow *ShellUi::launcherWindow() const
{
    return m_launcherWindow;
}

void ShellUi::updateScreenGeometry(const QRect &screenGeometry)
{
#if 0
    // Get available geometry
    QRectF geo = m_rootObject->property("availableGeometry").toRectF();

    // Send available geometry to the compositor
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_set_available_geometry(
                object->shell, m_output,
                int(geo.x()), int(geo.y()),
                int(geo.width()), int(geo.height()));
#endif
}

#include "moc_shellui.cpp"
