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
#include <QtQml/QQmlEngine>

#include <qpa/qplatformnativeinterface.h>

#include "shellui.h"
#include "waylandintegration.h"

ShellUi::ShellUi(QQmlEngine *engine, QScreen *screen, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
    , m_screen(screen)
    , m_paintedWindows(0)
    , m_backgroundWindow(0)
    , m_panelWindow(0)
    , m_launcherWindow(0)
    , m_lockScreenWindow(0)
{
    // Native platform interface
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    // Get native wl_output for the current screen
    m_output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen));

    // Create grab window
    m_grabWindow = new GrabWindow(screen);
    m_grabWindow->show();

    // Create Background window
    m_backgroundWindow = new BackgroundWindow(this);
    connect(m_backgroundWindow, SIGNAL(visibleChanged(bool)),
            this, SLOT(visibleChanged(bool)));

    // Create Panel window
    m_panelWindow = new PanelWindow(this);
    connect(m_panelWindow, SIGNAL(visibleChanged(bool)),
            this, SLOT(visibleChanged(bool)));

    // Create Launcher window
    m_launcherWindow = new LauncherWindow(this);
    connect(m_launcherWindow, SIGNAL(visibleChanged(bool)),
            this, SLOT(visibleChanged(bool)));

    // React to screen size changes
    connect(screen, SIGNAL(geometryChanged(QRect)),
            this, SLOT(updateScreenGeometry(QRect)));

    // Send available geometry to the compositor
    updateScreenGeometry(screen->geometry());
}

ShellUi::~ShellUi()
{
    m_lockScreenWindow->close();
    m_panelWindow->close();
    m_launcherWindow->close();
    m_backgroundWindow->close();
}

QQmlEngine *ShellUi::engine() const
{
    return m_engine;
}

QScreen *ShellUi::screen() const
{
    return m_screen;
}

wl_output *ShellUi::output() const
{
    return m_output;
}

QRect ShellUi::availableGeometry() const
{
    return m_availableGeometry;
}

GrabWindow *ShellUi::grabWindow() const
{
    return m_grabWindow;
}

BackgroundWindow *ShellUi::backgroundWindow() const
{
    return m_backgroundWindow;
}

PanelWindow *ShellUi::panelWindow() const
{
    return m_panelWindow;
}

LauncherWindow *ShellUi::launcherWindow() const
{
    return m_launcherWindow;
}

LockScreenWindow *ShellUi::lockScreenWindow() const
{
    return m_lockScreenWindow;
}

void ShellUi::updateScreenGeometry(const QRect &rect)
{
    // Calculate available geometry
    QRect geometry = rect;
    geometry.setTop(m_panelWindow->geometry().top() +
                    m_panelWindow->geometry().height());
    switch (m_launcherWindow->settings()->alignment()) {
    case LauncherSettings::LeftAlignment:
        geometry.setLeft(m_launcherWindow->geometry().left() +
                         m_launcherWindow->geometry().width());
        break;
    case LauncherSettings::RightAlignment:
        geometry.setRight(m_launcherWindow->geometry().right() -
                          m_launcherWindow->geometry().width());
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
    return;
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_set_available_geometry(
                object->shell, m_output,
                geometry.x(), geometry.y(),
                geometry.width(), geometry.height());
}

void ShellUi::createLockScreenWindow()
{
    if (!m_lockScreenWindow)
        m_lockScreenWindow = new LockScreenWindow(this);
    m_lockScreenWindow->show();
    m_lockScreenWindow->setWindowType();
}

void ShellUi::closeLockScreenWindow()
{
    if (!m_lockScreenWindow)
        return;

    //m_lockScreenWindow->hide();
    m_lockScreenWindow->deleteLater();
    m_lockScreenWindow = 0;
}

void ShellUi::visibleChanged(bool visible)
{
    // Just return if all shell windows were shown because
    // our business is over
    if (m_paintedWindows == 3)
        return;

    // Every time a window is shown we increment the counter,
    // when we reach 3 (that is: Background, Panel and Launcher)
    // we emit the ready() signal to inform DesktopShell the
    // shell for this screen was painted
    if (visible)
        m_paintedWindows++;
    if (m_paintedWindows == 3)
        Q_EMIT ready();
}

#include "moc_shellui.cpp"
