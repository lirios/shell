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

#include <QtCore/QDebug>
#include <QtCore/QCommandLineParser>
#include <QtQml/QQmlDebuggingEnabler>
#include <QtWidgets/QApplication>

#include "config.h"
#include "gitsha1.h"
#include "shellmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii Shell"));
    app.setApplicationVersion(QStringLiteral(HAWAII_SHELL_VERSION_STRING));
    app.setOrganizationDomain(QStringLiteral("hawaii.org"));
    app.setOrganizationName(QStringLiteral("Hawaii"));
    app.setQuitOnLastWindowClosed(false);

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Hawaii Shell"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Synthesize touch for unhandled mouse events
    QCommandLineOption dbgOption(QStringList() << QStringLiteral("d") << QStringLiteral("qmljsdebugger"),
                                 QCoreApplication::translate("Command line parser", "Enable QML JavaScript debugger."));
    parser.addOption(dbgOption);

    // Parse command line
    parser.process(app);
    if (parser.isSet(dbgOption)) {
        QQmlDebuggingEnabler enabler;
        Q_UNUSED(enabler);
    }

    // Print version information
    qDebug() << qPrintable(QStringLiteral("== Hawaii Shell v%1 ==\n").arg(HAWAII_SHELL_VERSION_STRING))
             << "** http://www.maui-project.org\n"
             << "** Bug reports to: https://github.com/mauios/hawaii-shell/issues\n"
             << qPrintable(QStringLiteral("** Build: %1-%2")
                           .arg(HAWAII_SHELL_VERSION_STRING).arg(GIT_REV));

    // Create the main shell object
    ShellManager::instance();

    return app.exec();
}
