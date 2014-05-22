/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickWindow>

#include "config.h"
#include "notificationsdaemon.h"
#include "registrylistener.h"

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

int main(int argc, char *argv[])
{
    // Force wayland QPA plugin
    setenv("QT_QPA_PLATFORM", "wayland", 1);

    // Setup application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii Notifications Daemon"));
    app.setApplicationVersion(QStringLiteral(HAWAII_SHELL_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("hawaii.org"));
    app.setOrganizationName(QStringLiteral("Hawaii"));

    // This runs only on Wayland
    if (!app.platformName().contains(QStringLiteral("wayland"))) {
        const char *msg = "Hawaii's notifications daemon runs only on "
                "Wayland, please pass the -platform wayland argument";
#if HAVE_SYSTEMD
        sd_notifyf(0,
                   "STATUS=Failed to start up: %s\n"
                   "ERRNO=%i",
                   msg, EFAULT);
#endif
        qFatal("%s", msg);
    }

    // Windows need the alpha buffer
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Create the daemon instance
    NotificationsDaemon::instance();

    // Listen to Wayland registry events
    RegistryListener::instance()->run();

    return app.exec();
}
