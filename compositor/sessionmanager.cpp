/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL3-HAWAII$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 or any later version accepted
 * by Pier Luigi Fiorini, which shall act as a proxy defined in Section 14
 * of version 3 of the license.
 *
 * Any modifications to this file must keep this entire header intact.
 *
 * The interactive user interfaces in modified source and object code
 * versions of this program must display Appropriate Legal Notices,
 * as required under Section 5 of the GNU General Public License version 3.
 *
 * In accordance with Section 7(b) of the GNU General Public License
 * version 3, these Appropriate Legal Notices must retain the display of the
 * "Powered by Hawaii" logo.  If the display of the logo is not reasonably
 * feasible for technical reasons, the Appropriate Legal Notices must display
 * the words "Powered by Hawaii".
 *
 * In accordance with Section 7(c) of the GNU General Public License
 * version 3, modified source and object code versions of this program
 * must be marked in reasonable ways as different from the original version.
 *
 * In accordance with Section 7(d) of the GNU General Public License
 * version 3, neither the "Hawaii" name, nor the name of any project that is
 * related to it, nor the names of its contributors may be used to endorse or
 * promote products derived from this software without specific prior written
 * permission.
 *
 * In accordance with Section 7(e) of the GNU General Public License
 * version 3, this license does not grant any license or rights to use the
 * "Hawaii" name or logo, nor any other trademark.
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
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include <GreenIsland/Compositor>

#include <qt5xdg/xdgautostart.h>
#include <qt5xdg/xdgdesktopfile.h>

#include "cmakedirs.h"
#include "loginmanager/loginmanager.h"
#include "powermanager/powermanager.h"
#include "processlauncher.h"
#include "processlauncheradaptor.h"
#include "screensaver.h"
#include "screensaveradaptor.h"
#include "sessionmanager.h"

#include <sys/types.h>
#include <signal.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "hawaii.session.manager")

using namespace GreenIsland;

SessionManager::SessionManager(QObject *parent)
    : QObject(parent)
    , m_loginManager(new LoginManager(this, this))
    , m_powerManager(new PowerManager(this))
    , m_launcher(new ProcessLauncher(this))
    , m_screenSaver(new ScreenSaver(this))
    , m_idle(false)
    , m_locked(false)
{
    // Lock and unlock the session
    connect(m_loginManager, &LoginManager::sessionLocked, this, [this] {
        setLocked(true);
    });
    connect(m_loginManager, &LoginManager::sessionUnlocked, this, [this] {
        setLocked(false);
    });

    // Logout session before the system goes off
    connect(m_loginManager, &LoginManager::logOutRequested,
            this, &SessionManager::logOut);
}

bool SessionManager::initialize()
{
    // Register D-Bus services
    if (!registerDBus())
        return false;

    return true;
}

bool SessionManager::isIdle() const
{
    return m_idle;
}

void SessionManager::setIdle(bool value)
{
    if (m_idle == value)
        return;

    m_idle = value;
    m_loginManager->setIdle(m_idle);
    Q_EMIT idleChanged(value);
}

void SessionManager::idleInhibit()
{
    Compositor::instance()->incrementIdleInhibit();
}

void SessionManager::idleUninhibit()
{
    Compositor::instance()->decrementIdleInhibit();
}

bool SessionManager::isLocked() const
{
    return m_locked;
}

void SessionManager::setLocked(bool value)
{
    if (m_locked == value)
        return;

    m_locked = value;
    Q_EMIT lockedChanged(value);
}

void SessionManager::setupEnvironment()
{
    // Set paths only if we are installed onto a non standard location
    QString path;

    if (qEnvironmentVariableIsSet("PATH")) {
        path = QStringLiteral("%1:%2").arg(INSTALL_BINDIR).arg(QString(qgetenv("PATH")));
        qputenv("PATH", path.toUtf8());
    }

    if (qEnvironmentVariableIsSet("QT_PLUGIN_PATH")) {
        path = QStringLiteral("%1:%2").arg(INSTALL_PLUGINDIR).arg(QString(qgetenv("QT_PLUGIN_PATH")));
        qputenv("QT_PLUGIN_PATH", path.toUtf8());
    }

    if (qEnvironmentVariableIsSet("QML2_IMPORT_PATH")) {
        path = QStringLiteral("%1:%2").arg(INSTALL_QMLDIR).arg(QString(qgetenv("QML2_IMPORT_PATH")));
        qputenv("QML2_IMPORT_PATH", path.toUtf8());
    }

    if (qEnvironmentVariableIsSet("XDG_DATA_DIRS")) {
        path = QStringLiteral("%1:%2").arg(INSTALL_DATADIR).arg(QString(qgetenv("XDG_DATA_DIRS")));
        qputenv("XDG_DATA_DIRS", path.toUtf8());
    }

    if (qEnvironmentVariableIsSet("XDG_CONFIG_DIRS")) {
        path = QStringLiteral("%1:%2:/etc/xdg").arg(INSTALL_CONFIGDIR).arg(QString(qgetenv("XDG_CONFIG_DIRS")));
        qputenv("XDG_CONFIG_DIRS", path.toUtf8());
    }

    if (qEnvironmentVariableIsSet("XCURSOR_PATH")) {
       path = QStringLiteral("%1:%2").arg(INSTALL_DATADIR "/icons").arg(QString(qgetenv("XCURSOR_PATH")));
        qputenv("XCURSOR_PATH", path.toUtf8());
    }

    // Set XDG environment variables
    if (qEnvironmentVariableIsEmpty("XDG_DATA_HOME")) {
        QString path = QStringLiteral("%1/.local/share").arg(QString(qgetenv("HOME")));
        qputenv("XDG_DATA_HOME", path.toUtf8());
    }
    if (qEnvironmentVariableIsEmpty("XDG_CONFIG_HOME")) {
        QString path = QStringLiteral("%1/.config").arg(QString(qgetenv("HOME")));
        qputenv("XDG_CONFIG_HOME", path.toUtf8());
    }

    // Environment
    qputenv("QT_QPA_PLATFORMTHEME", QByteArrayLiteral("Hawaii"));
    qputenv("QT_QUICK_CONTROLS_STYLE", QByteArrayLiteral("Wind"));
    qputenv("XCURSOR_THEME", QByteArrayLiteral("hawaii"));
    qputenv("XCURSOR_SIZE", QByteArrayLiteral("16"));
    qputenv("QSG_RENDER_LOOP", QByteArrayLiteral("windows"));
}

bool SessionManager::registerDBus()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    // Process launcher
    new ProcessLauncherAdaptor(m_launcher);
    if (!bus.registerObject(QStringLiteral("/ProcessLauncher"), m_launcher)) {
        qCWarning(SESSION_MANAGER,
                  "Couldn't register /ProcessLauncher D-Bus object: %s",
                  qPrintable(bus.lastError().message()));
        return false;
    }

    // Screen saver
    new ScreenSaverAdaptor(m_screenSaver);
    if (!bus.registerObject(QStringLiteral("/org/freedesktop/ScreenSaver"), m_screenSaver)) {
        qCWarning(SESSION_MANAGER,
                  "Couldn't register /org/freedesktop/ScreenSaver D-Bus object: %s",
                  qPrintable(bus.lastError().message()));
        return false;
    }

    // Service
    if (!bus.registerService(QStringLiteral("org.hawaiios.Session"))) {
        qCWarning(SESSION_MANAGER,
                  "Couldn't register org.hawaiios.Session D-Bus service: %s",
                  qPrintable(bus.lastError().message()));
        return false;
    }

    return true;
}

void SessionManager::autostart()
{
    Q_FOREACH (const XdgDesktopFile &entry, XdgAutoStart::desktopFileList()) {
        if (!entry.isSuitable(true, QStringLiteral("X-Hawaii")))
            continue;

        qCDebug(SESSION_MANAGER) << "Autostart:" << entry.name() << "from" << entry.fileName();
        m_launcher->launchEntry(const_cast<XdgDesktopFile *>(&entry));
    }
}

void SessionManager::logOut()
{
    // Close all applications we launched
    m_launcher->closeApplications();

    // Exit
    QCoreApplication::quit();
}

#include "moc_sessionmanager.cpp"
