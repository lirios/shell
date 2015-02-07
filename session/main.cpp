/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QLoggingCategory>
#include <QtCore/QProcess>
#include <QtDBus/QDBusInterface>

#include "sigwatch/sigwatch.h"

#include "cmakedirs.h"
#include "config.h"
#include "processcontroller.h"
#include "sessionmanager.h"

#include <unistd.h>

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

int main(int argc, char *argv[])
{
    // Application
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));
    app.setApplicationVersion(QStringLiteral(HAWAII_VERSION_STRING));
    app.setOrganizationName(QStringLiteral("Hawaii"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Prepares the environment for Hawaii and launches it"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Mode
    QCommandLineOption modeOption(QStringList() << QStringLiteral("m") << QStringLiteral("mode"),
                                  TR("Specify session mode (possible values: eglfs, hwcomposer, nested)."), TR("mode"));
    parser.addOption(modeOption);

    // Logout
    QCommandLineOption logoutOption(QStringLiteral("logout"), TR("Quit from an existing session."));
    parser.addOption(logoutOption);

    // Parse command line
    parser.process(app);

    // Logout from an existing session
    if (parser.isSet(logoutOption)) {
        QDBusInterface interface(SessionManager::interfaceName, SessionManager::objectPath);
        interface.call("logOut");
        return 0;
    }

    // Empty mode is allowed only on X11 because we don't need much
    if (parser.value(modeOption).isEmpty()) {
        if (qEnvironmentVariableIsEmpty("DISPLAY"))
            qFatal("Invalid mode specify, possible values are: nested or eglfs.\n" \
                   "Empty mode is allowed only on X11.");
    } else {
        const QString mode = parser.value(modeOption);
        if (mode != QStringLiteral("eglfs") &&
                mode != QStringLiteral("hwcomposer") &&
                mode != QStringLiteral("nested"))
            qFatal("Invalid mode \"%s\"!", qPrintable(mode));
    }

    // Process controller that manages the compositor
    ProcessController processController(parser.value(modeOption));

    // Session manager
    SessionManager *sessionManager = new SessionManager(&processController);
    sessionManager->setupEnvironment();

    // Restart with D-Bus session if necessary
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qDebug() << "No D-Bus session bus available, respawning with dbus-launch...";

        QStringList args = QStringList()
                << QStringLiteral("--exit-with-session")
                << QCoreApplication::arguments();

        char **const argv_pointers = new char *[args.count() + 2];
        char **p = argv_pointers;

        *p = ::strdup("dbus-launch");
        ++p;

        for (const QString &arg: args) {
            *p = new char[arg.length() + 1];
            ::strcpy(*p, qPrintable(arg));
            ++p;
        }

        *p = 0;

        // Respawn with D-Bus session bus
        if (::execvp(argv_pointers[0], argv_pointers) == -1) {
            // If we are here execvp failed so we cleanup and bail out
            qWarning("Failed to respawn the session: %s", strerror(errno));

            p = argv_pointers;
            while (*p != 0)
                delete [] *p++;
            delete [] argv_pointers;

            ::exit(EXIT_FAILURE);
        }

        ::exit(EXIT_SUCCESS);
    }

    // Unix signals watcher
    UnixSignalWatcher sigwatch;
    sigwatch.watchForSignal(SIGINT);
    sigwatch.watchForSignal(SIGTERM);

    // Log out the session for SIGINT and SIGTERM
    QObject::connect(&sigwatch, &UnixSignalWatcher::unixSignal, [sessionManager](int signum) {
        qDebug() << "Log out caused by signal" << signum;
        sessionManager->logOut();
        QCoreApplication::quit();
    });

    // Register all D-Bus services
    if (!sessionManager->registerDBus())
        return 1;

    // Start the compositor
    processController.start();

    return app.exec();
}
