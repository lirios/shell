/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <greenisland/homeapplication.h>

int main(int argc, char *argv[])
{
    // Application
    GreenIsland::HomeApplication app(argc, argv);

    // Enable debug messages
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true"));
    QLoggingCategory::setFilterRules(QStringLiteral("*.warning=true"));
    QLoggingCategory::setFilterRules(QStringLiteral("*.critical=true"));

    // Set message pattern
    qSetMessagePattern("%{message}");

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("Command line parser", "Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Wayland socket
    QCommandLineOption socketOption(QStringList() << QStringLiteral("s") << QStringLiteral("socket"),
                                    QCoreApplication::translate("Command line parser", "Wayland socket"),
                                    QCoreApplication::translate("Command line parser", "name"));
    parser.addOption(socketOption);

    // Synthesize touch for unhandled mouse events
    QCommandLineOption synthesizeOption(QStringLiteral("synthesize-touch"),
                                        QCoreApplication::translate("Command line parser", "Synthesize touch for unhandled mouse events"));
    parser.addOption(synthesizeOption);

    // Idle time
    QCommandLineOption idleTimeOption(QStringList() << QStringLiteral("i") << QStringLiteral("idle-time"),
                                      QCoreApplication::translate("Command line parser", "Idle time in seconds (at least 5 seconds)"),
                                      QCoreApplication::translate("Command line parser", "secs"));
    idleTimeOption.setDefaultValue("300");
    parser.addOption(idleTimeOption);

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        QCoreApplication::translate("Command line parser", "Use fake screen configuration"),
                                        QCoreApplication::translate("Command line parser", "filename"));
    parser.addOption(fakeScreenOption);

    // Compositor package
    QCommandLineOption pluginOption(QStringList() << QStringLiteral("p") << QStringLiteral("compositor-plugin"),
                                    QCoreApplication::translate("Command line parser", "Force loading the given compositor plugin"),
                                    QStringLiteral("plugin"));
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

    return app.exec();
}
