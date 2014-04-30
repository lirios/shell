/****************************************************************************
 * This file is part of Hawaii Shell.
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
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QWindow>
#include <QtQml/QQmlApplicationEngine>

#include <GreenIsland/Utilities>

#include "compositor.h"
#include "config.h"
#include "logging.h"
#include "registration.h"

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

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

    // Register QML types
    registerQmlTypes();

    // Application engine
    QQmlApplicationEngine engine(QUrl("qrc:/qml/Compositor.qml"));
    QWindow *window = qobject_cast<QWindow *>(engine.rootObjects().first());
    if (!window) {
        qFatal("Compositor has no top level element that inhertis from Window!");
        return 1;
    }

    // Set screen
    window->setScreen(QGuiApplication::primaryScreen());

    // Set window geometry
    window->setGeometry(geometry);

    // Parse command line
    parser.process(app);
    if (parser.isSet(synthesizeOption))
        app.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);
    if (parser.isSet(fullScreenOption))
        window->setVisibility(QWindow::FullScreen);
    int idleInterval = parser.value(idleTimeOption).toInt();
    if (idleInterval >= 5)
        window->setProperty("idleInterval", idleInterval * 1000);

    // Show compositor window
    window->show();

#if HAVE_SYSTEMD
    sd_notifyf(0,
               "READY=1\n"
               "STATUS=Ready\n"
               "MAINPID=%llu",
               QCoreApplication::applicationPid());
#endif

    return app.exec();
}
