// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QFileInfo>
#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>

#if HAVE_SYS_PRCTL_H
#include <sys/prctl.h>
#endif

#ifdef HAVE_SYSTEMD
#  include <QTimer>
#  include <systemd/sd-daemon.h>
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

static void setupSystemd()
{
#ifdef HAVE_SYSTEMD
    auto *app = QCoreApplication::instance();

    QObject::connect(app, &QCoreApplication::aboutToQuit, app, [&] {
        sd_notify(0, "STOPPING=1");
    });

    uint64_t interval = 0;
    if (sd_watchdog_enabled(0, &interval) > 0) {
        if (interval > 0) {
            // Start a keep-alive timer every half of the watchdog interval,
            // and convert it from microseconds to milliseconds
            std::chrono::microseconds us(interval / 2);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(us);
            auto *timer = new QTimer(app);
            timer->setInterval(ms);
            QObject::connect(timer, &QTimer::timeout, app, [] {
                sd_notify(0, "WATCHDOG=1");
            });
            timer->start();
        }
    }

    sd_notify(0, "READY=1");
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
    qputenv("QT_WAYLAND_SHELL_INTEGRATION", "liri-layer-shell;xdg-shell");
    qputenv("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT", QByteArrayLiteral("1"));
    qputenv("QT_WAYLAND_DISABLE_DECORATION", "1");
    qunsetenv("QT_WAYLAND_DECORATION");

    // Application
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Shell Helper"));
    app.setApplicationVersion(QStringLiteral(LIRISHELL_VERSION));
    app.setOrganizationName(QStringLiteral("Liri"));
    app.setOrganizationDomain(QStringLiteral("liri.io"));
    app.setQuitOnLastWindowClosed(false);

    // Print version information
    qInfo("== Liri Shell Helper v%s ==\n"
          "** http://liri.io\n"
          "** Bug reports to: https://github.com/lirios/shell/issues\n"
          "** Build: %s-%s",
          LIRISHELL_VERSION, LIRISHELL_VERSION, GIT_REV);

    // Setup systemd
    setupSystemd();

    // Create UI
    QSharedPointer<QQmlApplicationEngine> engine(new QQmlApplicationEngine);
    engine->load(QStringLiteral("qrc:/qt/qml/helper/qml/main.qml"));

    return app.exec();
}
