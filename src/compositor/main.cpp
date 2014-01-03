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

#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <GreenIsland/Utilities>

#include "compositor.h"
#include "config.h"
#include "cmakedirs.h"
#include "logging.h"

int main(int argc, char *argv[])
{
    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName("Hawaii Compositor");
    app.setApplicationVersion(HAWAII_SHELL_VERSION_STRING);
    app.setOrganizationDomain("hawaii.org");
    app.setOrganizationName("Hawaii");

    // Check whether XDG_RUNTIME_DIR is ok or not
    GreenIsland::verifyXdgRuntimeDir();

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

    // Synthesize touch for unhandled mouse events
    QCommandLineOption synthesizeOption(QStringLiteral("synthesize-touch"),
                                        QCoreApplication::translate("Command line parser", "Synthesize touch for unhandled mouse events"));
    parser.addOption(synthesizeOption);

    // Full screen option
    QCommandLineOption fullScreenOption(QStringLiteral("fullscreen"),
                                        QCoreApplication::translate("Command line parser", "Full screen compositor window"));
    parser.addOption(fullScreenOption);

    // Idle time
    QCommandLineOption idleTimeOption(QStringList() << QStringLiteral("i") << QStringLiteral("idle-time"),
                                      QCoreApplication::translate("Command line parser", "Idle time in seconds (at least 5 seconds)"),
                                      QCoreApplication::translate("Command line parser", "secs"));
    idleTimeOption.setDefaultValue("300");
    parser.addOption(idleTimeOption);

    // Geometry
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    QRect geometry(screenGeometry.topLeft(), QSize(1920, 1080));

    // Create compositor, run shell client
    Compositor *compositor = Compositor::instance();
    compositor->setGeometry(geometry);
    compositor->setOutputGeometry(geometry);

    // Parse command line
    parser.process(app);
    if (parser.isSet(synthesizeOption))
        app.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);
    if (parser.isSet(fullScreenOption)) {
        compositor->setGeometry(screenGeometry);
        compositor->setVisibility(QWindow::FullScreen);
    }
    int idleInterval = parser.value(idleTimeOption).toInt();
    if (idleInterval >= 5)
        compositor->setIdleInterval(idleInterval * 1000);
    
    // Run shell client
    compositor->setShellFileName(QLatin1String(INSTALL_LIBEXECDIR "/starthawaii"));
    compositor->runShell();

    // Show compositor window
    compositor->show();

    return app.exec();
}
