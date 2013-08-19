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

#include <QtCore/QCoreApplication>

#include "desktopshell.h"
#include "lockscreenwindow.h"
#include "waylandintegration.h"
#include "sessionmanager.h"
#include "shellui.h"

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
{
}

void SessionManager::logout()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_quit(object->shell);
}

void SessionManager::lock()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_lock(object->shell);
}

void SessionManager::unlock()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_unlock(object->shell);

    // Close all lock screens, this will unmap the window and
    // save some resources
    foreach (ShellUi *ui, DesktopShell::instance()->windows())
        ui->closeLockScreenWindow();

    // Synchronization
    while (QCoreApplication::hasPendingEvents())
        QCoreApplication::processEvents();
}

#include "moc_sessionmanager.cpp"
