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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtGui/QGuiApplication>

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

static const QEvent::Type StartupEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

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

    // Home application
    m_homeApp = new HomeApplication(this);

    // Process launcher
    m_launcher = new ProcessLauncher(this);

    // Session manager
    m_sessionManager = new SessionManager(this);

    // Fail safe mode
    connect(m_homeApp, &HomeApplication::objectCreated,
            this, &Application::objectCreated);

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

void Application::setScreenConfiguration(const QString &fakeScreenData)
{
    m_homeApp->setScreenConfiguration(fakeScreenData);
}

void Application::setUrl(const QUrl &url)
{
    m_url = url;
}

void Application::customEvent(QEvent *event)
{
    if (event->type() == StartupEventType)
        startup();
}

void Application::startup()
{
    // Can't do the startup sequence twice
    if (m_started)
        return;

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
    m_homeApp->setContextProperty(QLatin1String("platformName"),
                                  QGuiApplication::platformName());

    // Session interface
    m_homeApp->setContextProperty(QStringLiteral("SessionInterface"),
                                  m_sessionManager);

    // Load the compositor
    if (!m_homeApp->loadUrl(m_url))
        QCoreApplication::exit(1);

    // Set Wayland socket name
    QObject *rootObject = m_homeApp->rootObjects().at(0);
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
    m_launcher = Q_NULLPTR;

    m_homeApp->deleteLater();
    m_homeApp = Q_NULLPTR;

    m_sessionManager->deleteLater();
    m_sessionManager = Q_NULLPTR;
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
        m_homeApp->loadUrl(QUrl(QStringLiteral("qrc:/qml/error/ErrorCompositor.qml")));
    }
}

StartupEvent::StartupEvent()
    : QEvent(StartupEventType)
{
}

#include "moc_application.cpp"
