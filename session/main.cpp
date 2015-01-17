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
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>

#include "cmakedirs.h"
#include "config.h"
#include "processcontroller.h"
#include "sessionadaptor.h"
#include "sessionmanager.h"

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

const QString sessionService("org.hawaii.session");
const QString sessionObject("/HawaiiSession");

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
                                  TR("Specify session mode (possible values: nested, eglfs)."), TR("mode"));
    parser.addOption(modeOption);

    // Logout
    QCommandLineOption logoutOption(QStringLiteral("logout"), TR("Quit from an existing session."));
    parser.addOption(logoutOption);

    // Parse command line
    parser.process(app);

    // Logout from an existing session
    if (parser.isSet(logoutOption)) {
        QDBusInterface interface(sessionService, sessionObject);
        interface.call("logOut");
        return 0;
    }

    // Process controller that manages the compositor
    ProcessController processController(parser.value(modeOption));

    // Session manager
    SessionManager sessionManager(&processController);
    sessionManager.setupEnvironment();

    // Restart with D-Bus session if necessary
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qDebug() << "No D-Bus session bus available, respawning with dbus-launch...";

        qint64 pid = 0;
        bool ret = QProcess::startDetached(QStringLiteral("dbus-launch"), QStringList()
                                               << QStringLiteral("--exit-with-session")
                                               << QCoreApplication::arguments(),
                                           QString(), &pid);
        if (ret) {
            qDebug() << "New process:" << pid;
            return 0;
        } else {
            qWarning() << "Failed to respawn, aborting...";
            return 1;
        }
    }

    // Start the D-Bus service
    (void)new SessionAdaptor(&sessionManager);
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService(sessionService))
        qFatal("Couldn't register org.hawaii.session D-Bus service: %s",
               qPrintable(bus.lastError().message()));
    if (!bus.registerObject(sessionObject, &sessionManager))
        qFatal("Couldn't register /HawaiiSession D-Bus object: %s",
               qPrintable(bus.lastError().message()));

    // Start the compositor
    processController.start();

    return app.exec();
}
