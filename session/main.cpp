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
#include "compositorlauncher.h"
#include "config.h"
#include "gitsha1.h"
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

    // Login manager
    QCommandLineOption lmOption(QStringLiteral("from-login-manager"),
                                TR("Specify the session is launched from a login manager (for example gdm or sddm)."));
    parser.addOption(lmOption);

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

    // Process controller that manages the compositor
    CompositorLauncher *launcher = CompositorLauncher::instance();
    QString mode = parser.value(modeOption);
    if (mode == QStringLiteral("eglfs"))
        launcher->setMode(CompositorLauncher::EglFSMode);
    else if (mode == QStringLiteral("hwcomposer"))
        launcher->setMode(CompositorLauncher::HwComposerMode);
    else if (mode == QStringLiteral("nested"))
        launcher->setMode(CompositorLauncher::NestedMode);
    else if (!mode.isEmpty()) {
        qWarning() << "Invalid mode argument" << mode;
        return 1;
    }

    // Session manager
    SessionManager *sessionManager = SessionManager::instance();
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
    });

    // Print version information
    qDebug("== Hawaii Session v%s ==\n"
           "** https://hawaii-desktop.github.io\n"
           "** Bug reports to: https://github.com/hawaii-desktop/hawaii-shell/issues\n"
           "** Build: %s-%s",
           HAWAII_VERSION_STRING, HAWAII_VERSION_STRING, GIT_REV);

    // Register all D-Bus services
    if (!sessionManager->registerDBus())
        return 1;

    // Start the compositor
    launcher->start(parser.isSet(lmOption));

    return app.exec();
}
