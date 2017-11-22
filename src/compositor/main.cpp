/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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
#include <QGuiApplication>
#include <QQuickStyle>

#include <Qt5GSettings/QGSettings>

#include "gitsha1.h"
#include "application.h"

#if HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include <signal.h>

#define TR(x) QT_TRANSLATE_NOOP("Command line parser", QStringLiteral(x))

static void disablePtrace()
{
#if !defined(DEVELOPMENT_BUILD) && defined(PR_SET_DUMPABLE)
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
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("liri"));
    qputenv("QT_QUICK_CONTROLS_1_STYLE", QByteArrayLiteral("Flat"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("material"));
    qputenv("QT_WAYLAND_DECORATION", QByteArrayLiteral("material"));
    qputenv("XCURSOR_THEME", QByteArrayLiteral("Paper"));
    qputenv("XCURSOR_SIZE", QByteArrayLiteral("16"));
    qputenv("XDG_MENU_PREFIX", QByteArrayLiteral("liri-"));
    qputenv("XDG_CURRENT_DESKTOP", QByteArrayLiteral("X-Liri"));
    qunsetenv("QT_WAYLAND_DISABLE_WINDOWDECORATION");

    // Load input method
    QtGSettings::QGSettings settings(QStringLiteral("io.liri.desktop.interface"),
                                     QStringLiteral("/io/liri/desktop/interface/"));
    qputenv("QT_IM_MODULE", settings.value(QStringLiteral("inputMethod")).toByteArray());
}

int main(int argc, char *argv[])
{
    // Disable ptrace except for gdb
    disablePtrace();

    // Setup the environment
    setupEnvironment();

    // Force liri QPA for the compositor
    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("liri"));

    // Disable QPA mouse cursor
    qputenv("QT_QPA_EGLFS_HIDECURSOR", QByteArrayLiteral("1"));

    // Automatically support HiDPI
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Shell"));
    app.setApplicationVersion(QStringLiteral(LIRISHELL_VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));
    app.setFallbackSessionManagementEnabled(false);
    app.setQuitOnLastWindowClosed(false);

    // Enable scene graph logs
    QLoggingCategory::setFilterRules(QStringLiteral("qt.scenegraph.general=true"));

    // Set style
    QQuickStyle::setStyle(QStringLiteral("Material"));

    // Command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription(TR("Wayland compositor and shell for Liri OS"));
    parser.addHelpOption();
    parser.addVersionOption();

    // Fake screen configuration
    QCommandLineOption fakeScreenOption(QStringLiteral("fake-screen"),
                                        TR("Use fake screen configuration"),
                                        TR("filename"));
    parser.addOption(fakeScreenOption);

    QCommandLineOption noAutostartOption(QStringLiteral("no-autostart"),
                                         TR("Do not run autostart programs"));
    parser.addOption(noAutostartOption);

    // Raise SIGSTOP on start-up
    QCommandLineOption waitForDebuggerOption(QStringLiteral("wait-for-debugger"),
                                             TR("Raise SIGSTOP on startup"));
    parser.addOption(waitForDebuggerOption);

#ifdef DEVELOPMENT_BUILD
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
        qCritical("No D-Bus session bus available, please run Liri Shell with dbus-launch.");
        return 1;
    }

    // Arguments
    QString fakeScreenData = parser.value(fakeScreenOption);

    // Wait for debugger
    if (parser.isSet(waitForDebuggerOption)) {
        qWarning("Waiting for debugger on PID %lld, send SIGCONT to continue...",
                 QCoreApplication::applicationPid());
        ::raise(SIGSTOP);
    }

    // Print version information
    qInfo("== Liri Shell v%s ==\n"
          "** http://liri.io\n"
          "** Bug reports to: https://github.com/lirios/shell/issues\n"
          "** Build: %s-%s",
          LIRISHELL_VERSION, LIRISHELL_VERSION, GIT_REV);

    // Print OS information
    qInfo("%s", qPrintable(Application::systemInformation().trimmed()));

    // Application
    Application *shell = new Application();
    shell->setAutostartEnabled(!parser.isSet(noAutostartOption));
    shell->setScreenConfigurationFileName(fakeScreenData);

    // Create the compositor and run
    bool urlAlreadySet = false;
#ifdef DEVELOPMENT_BUILD
    if (parser.isSet(qmlOption)) {
        urlAlreadySet = true;
        shell->setUrl(QUrl::fromLocalFile(parser.value(qmlOption)));
    }
#endif
    if (!urlAlreadySet)
        shell->setUrl(QUrl(QStringLiteral("qrc:/qml/Compositor.qml")));
    QCoreApplication::postEvent(shell, new StartupEvent());

    return app.exec();
}
