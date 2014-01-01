/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <qpa/qplatformnativeinterface.h>

#include "hawaiishell.h"
#include "shellscreen.h"

ShellScreen::ShellScreen(QScreen *screen, QObject *parent)
    : QObject(parent)
    , m_screen(screen)
    , m_backgroundWindow(0)
    , m_launcherWindow(0)
    , m_overlayWindow(0)
{
    // Native platform interface
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    // Get native wl_output for the current screen
    m_output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen));

    // Create Background window
    m_backgroundWindow = new BackgroundWindow(this);
    m_backgroundWindow->show();

    // Create Launcher window
    m_launcherWindow = new LauncherWindow(this);
    m_launcherWindow->show();

    // Create Overlay window
    m_overlayWindow = new OverlayWindow(this);
    m_overlayWindow->show();

    // React to screen size changes
    connect(screen, SIGNAL(geometryChanged(QRect)),
            this, SLOT(updateScreenGeometry(QRect)));

    // Send available geometry to the compositor
    updateScreenGeometry(screen->geometry());
}


QScreen *ShellScreen::screen() const
{
    return m_screen;
}

wl_output *ShellScreen::output() const
{
    return m_output;
}

QRect ShellScreen::availableGeometry() const
{
    return m_availableGeometry;
}

BackgroundWindow *ShellScreen::backgroundWindow() const
{
    return m_backgroundWindow;
}

LauncherWindow *ShellScreen::launcherWindow() const
{
    return m_launcherWindow;
}

OverlayWindow *ShellScreen::overlayWindow() const
{
    return m_overlayWindow;
}

void ShellScreen::updateScreenGeometry(const QRect &rect)
{
    // Calculate available geometry
    QRect geometry = rect;
    switch (m_launcherWindow->settings()->alignment()) {
    case LauncherSettings::LeftAlignment:
        geometry.setLeft(m_launcherWindow->geometry().left() +
                         m_launcherWindow->geometry().width());
        break;
    case LauncherSettings::RightAlignment:
        geometry.setRight(m_launcherWindow->geometry().right() -
                          m_launcherWindow->geometry().width());
        break;
    case LauncherSettings::TopAlignment:
        geometry.setTop(m_launcherWindow->geometry().top() +
                        m_launcherWindow->geometry().height());
        break;
    case LauncherSettings::BottomAlignment:
        geometry.setBottom(rect.bottom() -
                           m_launcherWindow->geometry().height());
        break;
    }

    // Save it and notify observers
    m_availableGeometry = geometry;
    Q_EMIT availableGeometryChanged(geometry);

    // Send available geometry to the compositor
    HawaiiShell::instance()->setAvailableGeometry(screen(), geometry);
}

#include "moc_shellscreen.cpp"
