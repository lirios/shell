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

#include <QDir>
#include <QSysInfo>
#include <QtCore/private/qsimd_p.h>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QStandardPaths>

#include <qt5xdg/xdgautostart.h>
#include <qt5xdg/xdgdesktopfile.h>

#include <QtWaylandCompositor/QWaylandCompositor>

#include "application.h"
#include "processlauncher/processlauncher.h"
#include "sessionmanager/sessionmanager.h"
#include "sigwatch.h"

#if HAVE_SYSTEMD
#  include <systemd/sd-daemon.h>
#endif

#include <unistd.h>
#include <sys/types.h>

#define DUMP_CPU_FEATURE(feature, name)  \
    if (qCpuHasFeature(feature)) str << " " name;

static const QEvent::Type StartupEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

static int convertPermission(const QFileInfo &fileInfo)
{
    int p = 0;

    if (fileInfo.permission(QFile::ReadUser))
        p += 400;
    if (fileInfo.permission(QFile::WriteUser))
        p += 200;
    if (fileInfo.permission(QFile::ExeUser))
        p += 100;
    if (fileInfo.permission(QFile::ReadGroup))
        p += 40;
    if (fileInfo.permission(QFile::WriteGroup))
        p += 20;
    if (fileInfo.permission(QFile::ExeGroup))
        p += 10;
    if (fileInfo.permission(QFile::ReadOther))
        p += 4;
    if (fileInfo.permission(QFile::WriteOther))
        p += 2;
    if (fileInfo.permission(QFile::ExeOther))
        p += 1;

    return p;
}

Application::Application(QObject *parent)
    : QObject(parent)
    , m_failSafe(false)
    , m_started(false)
    , m_autostartEnabled(true)
{
    // Unix signals watcher
    UnixSignalWatcher *sigwatch = new UnixSignalWatcher(this);
    sigwatch->watchForSignal(SIGINT);
    sigwatch->watchForSignal(SIGTERM);

    // Quit when the process is killed
    connect(sigwatch, &UnixSignalWatcher::unixSignal, this, &Application::unixSignal);

    // Application engine
    m_appEngine = new QQmlApplicationEngine(this);
    connect(m_appEngine, &QQmlApplicationEngine::objectCreated,
            this, &Application::objectCreated);

    // Process launcher
    m_launcher = new ProcessLauncher(this);

    // Session manager
    m_sessionManager = new SessionManager(this);

    // Invoke shutdown sequence when quitting
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
            this, &Application::shutdown);
}

bool Application::isAutostartEnabled() const
{
    return m_autostartEnabled;
}

void Application::setAutostartEnabled(bool enabled)
{
    m_autostartEnabled = enabled;
}

QString Application::screenConfigurationFileName() const
{
    return m_screenConfigFileName;
}

void Application::setScreenConfigurationFileName(const QString &fileName)
{
    m_screenConfigFileName = fileName;
}

void Application::setUrl(const QUrl &url)
{
    m_url = url;
}

QString Application::systemInformation()
{
    QString result;
    QTextStream str(&result);

    str << "OS: " << QSysInfo::prettyProductName();
    str << " ["
        << QSysInfo::kernelType()
        << " version " << QSysInfo::kernelVersion()
        << "]\n";
    str << "Architecture: " << QSysInfo::currentCpuArchitecture() << "; ";
    str << "features:";
#if defined(Q_PROCESSOR_X86)
    DUMP_CPU_FEATURE(SSE2, "SSE2");
    DUMP_CPU_FEATURE(SSE3, "SSE3");
    DUMP_CPU_FEATURE(SSSE3, "SSSE3");
    DUMP_CPU_FEATURE(SSE4_1, "SSE4.1");
    DUMP_CPU_FEATURE(SSE4_2, "SSE4.2");
    DUMP_CPU_FEATURE(AVX, "AVX");
    DUMP_CPU_FEATURE(AVX2, "AVX2");
    DUMP_CPU_FEATURE(RTM, "RTM");
    DUMP_CPU_FEATURE(HLE, "HLE");
#elif defined(Q_PROCESSOR_ARM)
    DUMP_CPU_FEATURE(ARM_NEON, "Neon");
#elif defined(Q_PROCESSOR_MIPS)
    DUMP_CPU_FEATURE(DSP, "DSP");
    DUMP_CPU_FEATURE(DSPR2, "DSPR2");
#endif
    str << '\n';
    return result;
}

void Application::customEvent(QEvent *event)
{
    if (event->type() == StartupEventType)
        startup();
}

void Application::verifyXdgRuntimeDir()
{
    QByteArray dirName = qgetenv("XDG_RUNTIME_DIR");

    if (qEnvironmentVariableIsEmpty("XDG_RUNTIME_DIR")) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is not set.\n"
                    "Refer to your distribution on how to get it, or read\n"
                    "http://www.freedesktop.org/wiki/Specifications/basedir-spec\n"
                    "on how to implement it.\n");
        qFatal("%s", qPrintable(msg));
    }

    QFileInfo fileInfo(dirName);

    if (!fileInfo.exists()) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is set to "
                    "\"%1\", which doesn't exist.\n").arg(dirName.constData());
        qFatal("%s", qPrintable(msg));
    }

    if (convertPermission(fileInfo) != 700 || fileInfo.ownerId() != getuid()) {
        QString msg = QObject::tr(
                    "XDG_RUNTIME_DIR is set to \"%1\" and is not configured correctly.\n"
                    "Unix access mode must be 0700, but is 0%2.\n"
                    "It must also be owned by the current user (UID %3), "
                    "but is owned by UID %4 (\"%5\").\n")
                .arg(dirName.constData())
                .arg(convertPermission(fileInfo))
                .arg(getuid())
                .arg(fileInfo.ownerId())
                .arg(fileInfo.owner());
        qFatal("%s\n", qPrintable(msg));
    }
}

void Application::startup()
{
    // Can't do the startup sequence twice
    if (m_started)
        return;

    // Check whether XDG_RUNTIME_DIR is ok or not
    verifyXdgRuntimeDir();

    // Register D-Bus service
    if (!QDBusConnection::sessionBus().registerService(QStringLiteral("io.liri.Session"))) {
        qWarning("Failed to register D-Bus service: %s",
                 qPrintable(QDBusConnection::sessionBus().lastError().message()));
        QCoreApplication::exit(1);
    }

    // Session manager
    if (!m_sessionManager->registerWithDBus())
        QCoreApplication::exit(1);

    // Process launcher
    if (!ProcessLauncher::registerWithDBus(m_launcher))
        QCoreApplication::exit(1);

    // Set platform name
    m_appEngine->rootContext()->setContextProperty(QStringLiteral("platformName"),
                                                   QGuiApplication::platformName());

    // Set screen configuration file name
    m_appEngine->rootContext()->setContextProperty(QStringLiteral("screenConfigurationFileName"),
                                                   m_screenConfigFileName);

    // Session interface
    m_appEngine->rootContext()->setContextProperty(QStringLiteral("SessionInterface"),
                                                   m_sessionManager);

    // Load the compositor
    m_appEngine->load(m_url);

    // Set Wayland socket name
    QObject *rootObject = m_appEngine->rootObjects().at(0);
    QWaylandCompositor *compositor = qobject_cast<QWaylandCompositor *>(rootObject);
    if (compositor) {
#if HAVE_SYSTEMD
        connect(compositor, &QWaylandCompositor::createdChanged, this, [] {
            // Notify systemd when the Wayland socket is available
            sd_notify(0, "READY=1");
        });
#endif
        m_launcher->setWaylandSocketName(QString::fromUtf8(compositor->socketName()));

        // Set Qt platform for applications that will be executed from here
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("wayland"));

        // Set the Wayland socket name for clients
        qputenv("WAYLAND_DISPLAY", compositor->socketName());

        // Use xdg-shell-v5 for Qt clients
        qunsetenv("QT_WAYLAND_SHELL_INTEGRATION");
        qunsetenv("QT_WAYLAND_USE_XDG_SHELL");
        //qputenv("QT_WAYLAND_USE_XDG_SHELL", QByteArrayLiteral("1"));

        // Don't mess with client scale factor
        qunsetenv("QT_SCALE_FACTOR");
        qunsetenv("QT_SCREEN_SCALE_FACTORS");
        qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", QByteArrayLiteral("1"));
    }

    // Launch autostart applications
    autostart();

    m_started = true;
}

void Application::shutdown()
{
    m_launcher->deleteLater();
    m_launcher = nullptr;

    m_appEngine->deleteLater();
    m_appEngine = nullptr;

    m_sessionManager->deleteLater();
    m_sessionManager = nullptr;
}

void Application::autostart()
{
    if (!m_autostartEnabled) {
        qCDebug(SESSION_MANAGER) << "Autostart disabled";
        return;
    }

    Q_FOREACH (const XdgDesktopFile &entry, XdgAutoStart::desktopFileList()) {
        // Ignore entries that are explicitely not meant for Liri
        if (!entry.isSuitable(true, QStringLiteral("X-Liri")))
            continue;

        // If it's neither suitable for GNOME nor KDE then it's probably not meant
        // for us too, some utilities like those from XFCE have an explicit list
        // of desktop that are not supported instead of show them on XFCE
        //if (!entry.isSuitable(true, QLatin1String("GNOME")) && !entry.isSuitable(true, QLatin1String("KDE")))
        //continue;

        qCDebug(SESSION_MANAGER) << "Autostart:" << entry.name() << "from" << entry.fileName();
        m_launcher->launchEntry(entry);
    }
}

void Application::unixSignal()
{
    // Close all applications we launched
    m_launcher->closeApplications();

    // Exit
    QCoreApplication::quit();
}

void Application::objectCreated(QObject *object, const QUrl &)
{
    // All went fine
    if (object)
        return;

    // Compositor failed to load
    if (m_failSafe) {
        // We give up because even the error screen has an error
        qWarning("A fatal error has occurred while running Liri Shell, but the error "
                 "screen has errors too. Giving up.");
        QCoreApplication::exit(1);
    } else {
        // Load the error screen in case of error
        m_failSafe = true;
        m_appEngine->load(QUrl(QStringLiteral("qrc:/qml/error/ErrorCompositor.qml")));
    }
}

StartupEvent::StartupEvent()
    : QEvent(StartupEventType)
{
}

#include "moc_application.cpp"
