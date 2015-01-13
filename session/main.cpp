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

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QLoggingCategory>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include "cmakedirs.h"
#include "config.h"
#include "processcontroller.h"
#include "sessionadaptor.h"
#include "sessionmanager.h"

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
                                  TR("Specify session mode (possible values: nested, eglfs)."), TR("mode"));
    parser.addOption(modeOption);

    // Parse command line
    parser.process(app);

    // Process controller that manages the compositor
    ProcessController processController(parser.value(modeOption));

    // Session manager
    SessionManager sessionManager(&processController);
    (void)new SessionAdaptor(&sessionManager);
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.registerService("org.hawaii.session"))
        qFatal("Couldn't register org.hawaii.session D-Bus service: %s",
               qPrintable(bus.lastError().message()));
    if (!bus.registerObject("/HawaiiSession", &sessionManager))
        qFatal("Couldn't register /HawaiiSession D-Bus object: %s",
               qPrintable(bus.lastError().message()));

    // Start the compositor
    processController.start();

    return app.exec();
}
