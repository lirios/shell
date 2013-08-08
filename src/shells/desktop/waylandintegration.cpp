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

#include <QGuiApplication>
#include <QQuickWindow>

#include "waylandintegration.h"
#include "desktopshell.h"
#include "notificationsdaemon.h"
#include "shellui.h"

Q_GLOBAL_STATIC(WaylandIntegration, s_waylandIntegration)

const struct wl_registry_listener WaylandIntegration::registryListener = {
    WaylandIntegration::handleGlobal
};

const struct hawaii_desktop_shell_listener WaylandIntegration::shellListener = {
    WaylandIntegration::handlePresent,
    WaylandIntegration::handlePrepareLockSurface,
    WaylandIntegration::handleGrabCursor
};

WaylandIntegration::WaylandIntegration()
    : shell(0)
    , notification(0)
{
}

WaylandIntegration *WaylandIntegration::instance()
{
    return s_waylandIntegration();
}

void WaylandIntegration::handleGlobal(void *data,
                                      struct wl_registry *registry,
                                      uint32_t id,
                                      const char *interface,
                                      uint32_t version)
{
    Q_UNUSED(version);

    WaylandIntegration *object = static_cast<WaylandIntegration *>(data);

    if (strcmp(interface, "hawaii_desktop_shell") == 0) {
        // Bind interface and register listener
        object->shell = static_cast<struct hawaii_desktop_shell *>(
                    wl_registry_bind(registry, id, &hawaii_desktop_shell_interface, 1));
        hawaii_desktop_shell_add_listener(object->shell, &shellListener, data);

        // Create shell surfaces
        DesktopShell *shell = DesktopShell::instance();
        QMetaObject::invokeMethod(shell, "create");
    } else if (strcmp(interface, "wl_notification_daemon") == 0) {
        // Bind interface
        object->notification = static_cast<struct wl_notification_daemon *>(
                    wl_registry_bind(registry, id, &wl_notification_daemon_interface, 1));

        // Start the notifications daemon and connect to the session bus
        NotificationsDaemon *daemon = NotificationsDaemon::instance();
        QMetaObject::invokeMethod(daemon, "connectOnDBus");
    }
}

void WaylandIntegration::handlePresent(void *data,
                                       struct hawaii_desktop_shell *desktop_shell,
                                       struct wl_surface *surface)
{
    Q_UNUSED(data);
    Q_UNUSED(desktop_shell);

    DesktopShell *shell = DesktopShell::instance();

    foreach (ShellUi *shellUi, shell->windows()) {
        if (surface == shellUi->backgroundWindow()->surface()) {
            QMetaObject::invokeMethod(shellUi->backgroundWindow(), "showNormal");
        } else if (surface == shellUi->panelWindow()->surface()) {
            QMetaObject::invokeMethod(shellUi->panelWindow(), "show");
            QMetaObject::invokeMethod(shellUi->panelWindow(), "sendGeometry");
        } else if (surface == shellUi->launcherWindow()->surface()) {
            QMetaObject::invokeMethod(shellUi->launcherWindow(), "show");
            QMetaObject::invokeMethod(shellUi->launcherWindow(), "sendGeometry");
        }
    }
}

void WaylandIntegration::handlePrepareLockSurface(void *data,
                                                  struct hawaii_desktop_shell *desktop_shell)
{
    Q_UNUSED(data);

    DesktopShell *shell = DesktopShell::instance();

    // Create the lock screen only for the primary screen
    // TODO: Actually we should create one for each screen but this
    // requires protocol changes
    foreach (ShellUi *shellUi, shell->windows()) {
        if (shellUi->screen() == QGuiApplication::primaryScreen()) {
            if (shellUi->lockScreenWindow())
                QMetaObject::invokeMethod(shellUi->lockScreenWindow(), "setWindowType");
            else
                QMetaObject::invokeMethod(shellUi, "createLockScreenWindow");
            QMetaObject::invokeMethod(shellUi->lockScreenWindow(), "show");
            return;
        }
    }
}

void WaylandIntegration::handleGrabCursor(void *data,
                                          struct hawaii_desktop_shell *desktop_shell,
                                          uint32_t cursor)
{
    Q_UNUSED(data);
    Q_UNUSED(desktop_shell);

    QCursor qcursor;

    switch (cursor) {
    case HAWAII_DESKTOP_SHELL_CURSOR_NONE:
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_BUSY:
        qcursor.setShape(Qt::BusyCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_MOVE:
        qcursor.setShape(Qt::DragMoveCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM:
        qcursor.setShape(Qt::SizeVerCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_LEFT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_RIGHT:
        qcursor.setShape(Qt::SizeHorCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP_LEFT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_TOP_RIGHT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_LEFT:
        qcursor.setShape(Qt::SizeBDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_RESIZE_BOTTOM_RIGHT:
        qcursor.setShape(Qt::SizeFDiagCursor);
        break;
    case HAWAII_DESKTOP_SHELL_CURSOR_ARROW:
    default:
        qcursor.setShape(Qt::ArrowCursor);
        break;
    }

    DesktopShell *shell = DesktopShell::instance();

    foreach (ShellUi *shellUi, shell->windows())
        QMetaObject::invokeMethod(shellUi->grabWindow(), "setGrabCursor",
                                  Qt::QueuedConnection,
                                  Q_ARG(QCursor, qcursor));
}
