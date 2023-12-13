// SPDX-FileCopyrightText: 2022 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QFileInfo>
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>

#include "authenticator.h"
#include "extsessionlocksurfacev1.h"
#include "gitsha1.h"

#if HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

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

int main(int argc, char *argv[])
{
    // Disable ptrace except for gdb
    disablePtrace();

    // Setup the environment
    qputenv("XDG_SESSION_TYPE", QByteArrayLiteral("wayland"));
    qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("wayland"));
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("liri"));
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "liri-lockscreen");
    qputenv("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT", QByteArrayLiteral("1"));
    qputenv("QT_WAYLAND_DISABLE_DECORATION", "1");
    qunsetenv("QT_WAYLAND_DECORATION");
    //qunsetenv("WAYLAND_DISPLAY");

    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Liri Shell Lockscreen"));
    app.setApplicationVersion(QStringLiteral(LIRISHELL_VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));
    app.setQuitOnLastWindowClosed(false);

    // Print version information
    qInfo("== Liri Shell Lockscreen v%s ==\n"
          "** https://liri.io\n"
          "** Bug reports to: https://github.com/lirios/shell/issues\n"
          "** Build: %s-%s",
          LIRISHELL_VERSION, LIRISHELL_VERSION, GIT_REV);

    // Register QML types
    qmlRegisterSingletonType<Authenticator>("Liri.Shell.Client", 1, 0, "Authenticator",
                                            [](QQmlEngine *, QJSEngine *) -> QObject * {
        return new Authenticator();
    });
    qmlRegisterType<ExtSessionLockSurfaceV1>("Liri.Shell.Client", 1, 0, "ExtSessionLockSurfaceV1");

    // Create UI
    QSharedPointer<QQmlApplicationEngine> engine(new QQmlApplicationEngine);
    engine->load(QStringLiteral("qrc:/qml/main.qml"));

    return app.exec();
}
