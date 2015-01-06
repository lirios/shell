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

#include "cmakedirs.h"
#include "config.h"
#include "processcontroller.h"

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

    // Nest into another compositor
    QCommandLineOption nestedOption(QStringList() << QStringLiteral("n") << QStringLiteral("nested"),
                                    TR("Nest into another Wayland compositor"));
    parser.addOption(nestedOption);

    // Parse command line
    parser.process(app);

    // Process controller that manages the compositor
    ProcessController processController(parser.isSet(nestedOption));

    // Start the compositor
    processController.start();

    return app.exec();
}
