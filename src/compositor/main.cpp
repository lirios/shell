/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QCommandLineParser>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <GreenIsland/Utilities>

#include "compositor.h"
#include "config.h"

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

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Synthesize touch for unhandled mouse events
    QCommandLineOption synthesizeOption(QStringLiteral("synthesize-touch"),
                                        QObject::tr("Synthesize touch for unhandled mouse events"));
    parser.addOption(synthesizeOption);

    // Parse command line
    parser.process(app);
    if (parser.isSet(synthesizeOption))
        app.setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, true);

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    QRect geometry(screenGeometry.topLeft(), QSize(1920, 1080));

    Compositor compositor;
    compositor.setPosition(geometry.topLeft());
    compositor.resize(geometry.size());
    compositor.runShell();
    compositor.show();

    return app.exec();
}
