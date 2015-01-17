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
#include <QtCompositor/QWaylandOutput>

#include <GreenIsland/HomeApplication>
#include <GreenIsland/Compositor>

#include "sigwatch/sigwatch.h"

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Application
    GreenIsland::HomeApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

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

    // Compositor package
    QCommandLineOption pluginOption(QStringList() << QStringLiteral("p") << QStringLiteral("compositor-plugin"),
                                    TR("Force loading the given compositor plugin."), TR("plugin"));
    parser.addOption(pluginOption);

    // Parse command line
    parser.process(app);

    // Socket
    app.setSocket(parser.value(socketOption));

    // Fake screen data
    app.setFakeScreenData(parser.value(fakeScreenOption));

    // Idle timer
    int idleInterval = parser.value(idleTimeOption).toInt();
    if (idleInterval >= 5)
        app.setIdleTime(idleInterval * 1000);

    // Create the compositor and run
    if (!app.run(parser.value(pluginOption)))
        return 1;

    // Unix signals watcher
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);

    // Close all windows when the process is killed
    QObject::connect(&sigwatch, &UnixSignalWatcher::unixSignal,
                     &app, &GreenIsland::HomeApplication::quit);

    return app.exec();
}
