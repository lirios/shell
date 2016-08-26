/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2012-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
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
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QtWidgets/QApplication>
#include <QQuickStyle>

#include <Hawaii/greenisland_version.h>
#include <Hawaii/Settings/QGSettings>

#include "application.h"
#include "config.h"
#include "gitsha1.h"

#if HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

static void disablePtrace()
{
#if HAVE_PR_SET_DUMPABLE
    // Allow ptrace when running inside gdb
    const qint64 pid = QCoreApplication::applicationPid();
    const QFileInfo process(QStringLiteral("/proc/%1/exe").arg(pid));
    if (process.isSymLink() && process.symLinkTarget().endsWith(QLatin1String("/gdb")))
        return;

    ::prctl(PR_SET_DUMPABLE, 0);
#endif
}

static void setupEnvironment()
{
    // Set defaults
    if (qEnvironmentVariableIsEmpty("XDG_DATA_DIRS"))
        qputenv("XDG_DATA_DIRS", QByteArrayLiteral("/usr/local/share/:/usr/share/"));
    if (qEnvironmentVariableIsEmpty("XDG_CONFIG_DIRS"))
        qputenv("XDG_CONFIG_DIRS", QByteArrayLiteral("/etc/xdg"));

    // Environment
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("Hawaii"));
    qputenv("QT_QUICK_CONTROLS_1_STYLE", QByteArrayLiteral("Flat"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("material"));
    qputenv("QT_WAYLAND_DECORATION", QByteArrayLiteral("HawaiiMaterialDecoration"));
    qputenv("XCURSOR_THEME", QByteArrayLiteral("hawaii"));
    qputenv("XCURSOR_SIZE", QByteArrayLiteral("16"));
    qputenv("XDG_MENU_PREFIX", QByteArrayLiteral("hawaii-"));
    qputenv("XDG_CURRENT_DESKTOP", QByteArrayLiteral("X-Hawaii"));

    // Load input method
    Hawaii::QGSettings settings(QStringLiteral("org.hawaiios.desktop.interface"),
                                QStringLiteral("/org/hawaiios/desktop/interface/"));
    qputenv("QT_IM_MODULE", settings.value(QStringLiteral("inputMethod")).toByteArray());
}

int main(int argc, char *argv[])
{
    // Disable ptrace except for gdb
    disablePtrace();

    // Setup the environment
    setupEnvironment();

    // Automatically support HiDPI
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Application
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Hawaii"));
    app.setApplicationVersion(QStringLiteral(HAWAII_VERSION_STRING));
    app.setOrganizationName(QStringLiteral("Hawaii"));
    app.setOrganizationDomain(QStringLiteral("hawaiios.org"));
    app.setFallbackSessionManagementEnabled(false);
    app.setQuitOnLastWindowClosed(false);

    QQuickStyle::setStyle(QStringLiteral("Material"));

    // Set Qt platform for applications that will be executed from here
    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("wayland"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor for the Hawaii desktop environment"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Wayland socket
    QCommandLineOption socketOption(QStringLiteral("wayland-socket-name"),
                                    TR("Wayland socket"), TR("name"));
    parser.addOption(socketOption);

    // Nested mode
    QCommandLineOption nestedOption(QStringList() << QStringLiteral("n") << QStringLiteral("nested"),
                                    TR("Nest into a compositor that supports _wl_fullscreen_shell"));
    parser.addOption(nestedOption);

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        TR("Use fake screen configuration"),
                                        TR("filename"));
    parser.addOption(fakeScreenOption);

#if DEVELOPMENT_BUILD
    // Load shell from an arbitrary path
    QCommandLineOption qmlOption(QStringLiteral("qml"),
                                 QStringLiteral("Load a shell main QML file"),
                                 QStringLiteral("filename"));
    parser.addOption(qmlOption);
#endif

    // Parse command line
    parser.process(app);

    // Restart with D-Bus session if necessary
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qCritical("No D-Bus session bus available, please run Hawaii with dbus-launch.");
        return 1;
    }

    // Arguments
    bool nested = parser.isSet(nestedOption);
    QString socket = parser.value(socketOption);
    QString fakeScreenData = parser.value(fakeScreenOption);

    // Nested mode requires running from Wayland and a socket name
    // and fake screen data cannot be used
    if (nested) {
        if (!QGuiApplication::platformName().startsWith(QStringLiteral("greenisland"))) {
            qCritical("Nested mode only make sense when running on Wayland.\n"
                      "Please pass the \"-platform greenisland\" argument.");
            return 1;
        }

        if (socket.isEmpty()) {
            qCritical("Nested mode requires you to specify a socket name.\n"
                      "Please specify it with the \"--wayland-socket-name\" argument.");
            return 1;
        }

        if (!fakeScreenData.isEmpty()) {
            qCritical("Fake screen configuration cannot be used when nested");
            return 1;
        }
    }

    // Print version information
    qDebug("== Hawaii Compositor v%s (Green Island v%s) ==\n"
           "** http://hawaiios.org\n"
           "** Bug reports to: https://github.com/hawaii-desktop/hawaii-shell/issues\n"
           "** Build: %s-%s",
           HAWAII_VERSION_STRING, GREENISLAND_VERSION_STRING,
           HAWAII_VERSION_STRING, GIT_REV);

    // Application
    Application *hawaii = new Application();
    hawaii->setScreenConfiguration(fakeScreenData);

    // Create the compositor and run
    bool urlAlreadySet = false;
#if DEVELOPMENT_BUILD
    if (parser.isSet(qmlOption)) {
        urlAlreadySet = true;
        hawaii->setUrl(QUrl::fromLocalFile(parser.value(qmlOption)));
    }
#endif
    if (!urlAlreadySet)
        hawaii->setUrl(QUrl(QStringLiteral("qrc:/Compositor.qml")));
    QCoreApplication::postEvent(hawaii, new StartupEvent());

    return app.exec();
}
