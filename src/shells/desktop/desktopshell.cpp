/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlComponent>

#include <qpa/qplatformnativeinterface.h>

#include "applicationiconprovider.h"
#include "cmakedirs.h"
#include "declarativeplugin.h"
#include "desktopshell.h"
#include "notificationwindow.h"
#include "waylandintegration.h"
#include "shellui.h"
#include "shellwindow.h"

Q_GLOBAL_STATIC(DesktopShell, s_desktopShell)

DesktopShell::DesktopShell()
    : QObject()
{
    // Create QML engine
    m_engine = new QQmlEngine(this);

    // Register image provider
    m_engine->addImageProvider("appicon", new ApplicationIconProvider);

    // Register QML types
    registerQmlTypes();

    // Platform native interface
    QPlatformNativeInterface *native =
            QGuiApplication::platformNativeInterface();
    Q_ASSERT(native);

    // Get Wayland display
    m_display = static_cast<struct wl_display *>(
                native->nativeResourceForIntegration("display"));
    Q_ASSERT(m_display);

    // Display file descriptor
    m_fd = wl_display_get_fd(m_display);
    Q_ASSERT(m_fd > -1);
    qDebug() << "Wayland display socket:" << m_fd;

    // Wayland registry
    m_registry = wl_display_get_registry(m_display);
    Q_ASSERT(m_registry);

    // Wayland integration
    WaylandIntegration *integration = WaylandIntegration::instance();
    wl_registry_add_listener(m_registry, &WaylandIntegration::registryListener,
                             integration);
}

DesktopShell::~DesktopShell()
{
    foreach (ShellUi *shellUi, m_windows) {
        m_windows.removeOne(shellUi);
        delete shellUi;
    }

    delete m_engine;
}

DesktopShell *DesktopShell::instance()
{
    return s_desktopShell();
}

void DesktopShell::create()
{
    foreach (QScreen *screen, QGuiApplication::screens()) {
        ShellUi *ui = new ShellUi(screen, this);
        m_windows.append(ui);
    }
}

void DesktopShell::logout()
{

}

void DesktopShell::reboot()
{
}

void DesktopShell::powerOff()
{
}

#include "moc_desktopshell.cpp"
