/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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

#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QApplication>
#include <QtCompositor/QWaylandOutput>

#include <GreenIsland/Compositor>
#include <greenisland/greenisland_version.h>

#include "sigwatch/sigwatch.h"

#include "application.h"
#include "config.h"
#include "gitsha1.h"
#include "sessionmanager.h"

#include <unistd.h>

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Setup the environment
    SessionManager::setupEnvironment();

    // Application
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));
    app.setApplicationVersion(HAWAII_VERSION_STRING);
    app.setOrganizationName(QStringLiteral("Hawaii"));
    app.setOrganizationDomain(QStringLiteral("hawaiios.org"));
    app.setQuitOnLastWindowClosed(false);

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Wayland socket
    QCommandLineOption socketOption(QStringLiteral("wayland-socket-name"),
                                    TR("Wayland socket"), TR("name"));
    parser.addOption(socketOption);

    // Nested mode
    QCommandLineOption nestedOption(QStringList() << QStringLiteral("n") << QStringLiteral("nested"),
                                    TR("Nest into a compositor that supports _wl_fullscreen_shell"));
    parser.addOption(nestedOption);

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        TR("Use fake screen configuration"),
                                        TR("filename"));
    parser.addOption(fakeScreenOption);

    // Parse command line
    parser.process(app);

    // Restart with D-Bus session if necessary
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qCritical("No D-Bus session bus available, please run Hawaii with dbus-launch.");
        return 1;
    }

    // Arguments
    bool nested = parser.isSet(nestedOption);
    QString socket = parser.value(socketOption);
    QString fakeScreenData = parser.value(fakeScreenOption);

    // Nested mode requires running from Wayland and a socket name
    // and fake screen data cannot be used
    if (nested) {
        if (!QGuiApplication::platformName().startsWith(QStringLiteral("wayland"))) {
            qCritical("Nested mode only make sense when running on Wayland.\n"
                      "Please pass the \"-platform wayland\" argument.");
            return 1;
        }

        if (socket.isEmpty()) {
            qCritical("Nested mode requires you to specify a socket name.\n"
                      "Please specify it with the \"--wayland-socket-name\" argument.");
            return 1;
        }

        if (!fakeScreenData.isEmpty()) {
            qCritical("Fake screen configuration cannot be used when nested");
            return 1;
        }
    }

    // Unix signals watcher
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);

    // Quit when the process is killed
    QObject::connect(&sigwatch, &UnixSignalWatcher::unixSignal,
                     QApplication::instance(), &QApplication::quit);

    // Print version information
    qDebug("== Hawaii Compositor v%s (Green Island v%s) ==\n"
           "** http://hawaiios.org\n"
           "** Bug reports to: https://github.com/hawaii-desktop/hawaii-shell/issues\n"
           "** Build: %s-%s",
           HAWAII_VERSION_STRING, GREENISLAND_VERSION_STRING,
           HAWAII_VERSION_STRING, GIT_REV);

    // Home application
    Application homeApp;
    homeApp.setFakeScreenData(fakeScreenData);

    // Create the compositor and run
    if (!homeApp.run(nested, QStringLiteral("org.hawaiios.desktop")))
        return 1;

    return app.exec();
}
