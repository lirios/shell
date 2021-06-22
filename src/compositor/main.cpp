// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QLoggingCategory>
#include <QtCore/QCommandLineParser>
#include <QtCore/QFileInfo>
#include <QtCore/QStandardPaths>
#include <QGuiApplication>
#include <QLibraryInfo>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QTranslator>

#include <Qt5GSettings/QGSettings>

#include "gitsha1.h"
#include "application.h"

#if HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

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
    // Environment
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("liri"));

    // Load input method
    QtGSettings::QGSettings settings(QStringLiteral("io.liri.desktop.interface"),
                                     QStringLiteral("/io/liri/desktop/interface/"));
    qputenv("QT_IM_MODULE", settings.value(QStringLiteral("inputMethod")).toByteArray());
}

static void loadQtTranslations()
{
#ifndef QT_NO_TRANSLATION
    QString locale = QLocale::system().name();

    // Load Qt translations
    QTranslator *qtTranslator = new QTranslator(QCoreApplication::instance());
    if (qtTranslator->load(QStringLiteral("qt_%1").arg(locale), QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qApp->installTranslator(qtTranslator);
    } else {
        delete qtTranslator;
    }
#endif
}

static void loadAppTranslations()
{
#ifndef QT_NO_TRANSLATION
    QString locale = QLocale::system().name();

    // Find the translations directory
    const QString path = QLatin1String("liri-shell/translations");
    const QString translationsDir =
        QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                               path,
                               QStandardPaths::LocateDirectory);

    // Load shell translations
    QTranslator *appTranslator = new QTranslator(QCoreApplication::instance());
    if (appTranslator->load(QStringLiteral("%1/compositor_%2").arg(translationsDir, locale))) {
        QCoreApplication::installTranslator(appTranslator);
    } else if (locale == QLatin1String("C") ||
                locale.startsWith(QLatin1String("en"))) {
        // English is the default, it's translated anyway
        delete appTranslator;
    }
#endif
}

int main(int argc, char *argv[])
{
#ifndef DEVELOPMENT_BUILD
    if (::getuid() == 0) {
        qWarning("Liri Shell doesn't support running as root!");
        return 1;
    }
#endif

    // Disable ptrace except for gdb
    disablePtrace();

    // Setup the environment
    setupEnvironment();

    // Automatically detect the right platform plugin to use
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    // Since Qt 5.11.0 we can specify a fallback platform plugin,
    // so try wayland and xcb in this order unless it's running on a vt
    qputenv("QT_QPA_PLATFORM", "wayland;xcb;liri");
#else
    // Try to detect the platform based on environment variables,
    // fallback to liri if nothing is found
    if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("wayland"));
    else if (qEnvironmentVariableIsSet("DISPLAY"))
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("xcb"));
    else
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("liri"));
#endif

    // ShareOpenGLContexts is needed for using the threaded renderer
    // on NVIDIA EGLStreams and multi output compositors in general
    // (see QTBUG-63039 and QTBUG-87597)
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    // Automatically support HiDPI
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Shell"));
    app.setApplicationVersion(QStringLiteral(LIRISHELL_VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));
#ifndef QT_NO_SESSIONMANAGER
    app.setFallbackSessionManagementEnabled(false);
#endif
    app.setQuitOnLastWindowClosed(false);

    // Enable logs
    QLoggingCategory::setFilterRules(QStringLiteral("qt.scenegraph.general=true"));

    // Set style
    QQuickStyle::setStyle(QStringLiteral("Material"));

    // Load translations
    loadQtTranslations();
    loadAppTranslations();

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

    // Need a D-Bus session, that should have been set up by liri-session
    if (qEnvironmentVariableIsEmpty("DBUS_SESSION_BUS_ADDRESS")) {
        qWarning("No D-Bus session bus available, please don't manually run Liri Shell.");
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
    qInfo("== Liri Shell ==\n"
          "** https://liri.io\n"
          "** Bug reports to: https://github.com/lirios/shell/issues\n"
          "** Build: %s-%s",
          LIRISHELL_VERSION, GIT_REV);

    // Print OS information
    qInfo("Platform name: %s", qPrintable(QGuiApplication::platformName()));

    // Application
    Application *shell = new Application();
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
