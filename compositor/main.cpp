/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Application
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Session manager socket
    QCommandLineOption sessionSocketOption(QStringLiteral("session-socket"),
                                           TR("Session manager socket."),
                                           TR("name"));
    parser.addOption(sessionSocketOption);

    // Wayland socket
    QCommandLineOption socketOption(QStringList() << QStringLiteral("s") << QStringLiteral("socket"),
                                    TR("Wayland socket."), TR("name"));
    parser.addOption(socketOption);

    // Synthesize touch for unhandled mouse events
    QCommandLineOption synthesizeOption(QStringLiteral("synthesize-touch"),
                                        TR("Synthesize touch for unhandled mouse events."));
    parser.addOption(synthesizeOption);

    // Idle time
    QCommandLineOption idleTimeOption(QStringList() << QStringLiteral("i") << QStringLiteral("idle-time"),
                                      TR("Idle time in seconds (at least 5 seconds)."), TR("secs"));
    idleTimeOption.setDefaultValue("300");
    parser.addOption(idleTimeOption);

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        TR("Use fake screen configuration."), TR("filename"));
    parser.addOption(fakeScreenOption);

    // Parse command line
    parser.process(app);

    // Home application
    Application homeApp(parser.value(sessionSocketOption));

    // Socket
    homeApp.setSocket(parser.value(socketOption));

    // Fake screen data
    if (parser.isSet(fakeScreenOption))
        homeApp.setFakeScreenData(parser.value(fakeScreenOption));

    // Use default fake screen data on X11
    if (qEnvironmentVariableIsSet("DISPLAY") && homeApp.fakeScreenData().isEmpty()) {
        const QString fileName = QString("greenisland/screen-data/one-1920x1080.json");
        QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              fileName);
        if (!path.isEmpty())
            homeApp.setFakeScreenData(path);
    }

    // Idle timer
    int idleInterval = parser.value(idleTimeOption).toInt();
    if (idleInterval >= 5)
        homeApp.setIdleTime(idleInterval * 1000);

    // Print version information
    qDebug("== Hawaii Compositor v%s (Green Island v%s) ==\n"
           "** https://hawaii-desktop.github.io\n"
           "** Bug reports to: https://github.com/hawaii-desktop/hawaii-shell/issues\n"
           "** Build: %s-%s",
           HAWAII_VERSION_STRING, GREENISLAND_VERSION_STRING,
           HAWAII_VERSION_STRING, GIT_REV);

    // Create the compositor and run
    if (!homeApp.run(QStringLiteral("org.hawaii.desktop")))
        return 1;

    // Unix signals watcher
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);

    // Close all windows when the process is killed
    QObject::connect(&sigwatch, &UnixSignalWatcher::unixSignal, [=] {
        QApplication::quit();
    });

    return app.exec();
}
