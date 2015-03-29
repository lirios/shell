/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include <qt5xdg/xdgautostart.h>
#include <qt5xdg/xdgdesktopfile.h>

#include "cmakedirs.h"
#include "compositorlauncher.h"
#include "processlauncher.h"
#include "screensaver.h"
#include "sessionadaptor.h"
#include "sessionmanager.h"

#include <sys/types.h>
#include <signal.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "hawaii.session.manager")

SessionManager::SessionManager(CompositorLauncher *compositorLauncher)
    : QObject(compositorLauncher)
    , m_compositorLauncher(compositorLauncher)
    , m_launcher(new ProcessLauncher(this))
    , m_screenSaver(new ScreenSaver(this))
    , m_locked(false)
{
    // Autostart applications as soon as the compositor is ready
    connect(m_compositorLauncher, &CompositorLauncher::started, this, [this] {
        QTimer::singleShot(500, this, SLOT(autostart()));
    });

    // Log out when the compositor has finished
    connect(m_compositorLauncher, &CompositorLauncher::finished,
            this, &SessionManager::logOut);
}

void SessionManager::setupEnvironment()
{
    // Set paths only if we are installed onto a non standard location
    if (!QStringLiteral(INSTALL_PREFIX).startsWith(QStringLiteral("/usr"))) {
        QString path;

        path = QStringLiteral("%1:%2").arg(INSTALL_BINDIR).arg(QString(qgetenv("PATH")));
        qputenv("PATH", path.toUtf8());

        path = QStringLiteral("%1:%2").arg(INSTALL_PLUGINDIR).arg(QString(qgetenv("QT_PLUGIN_PATH")));
        qputenv("QT_PLUGIN_PATH", path.toUtf8());

        path = QStringLiteral("%1:%2").arg(INSTALL_QMLDIR).arg(QString(qgetenv("QML2_IMPORT_PATH")));
        qputenv("QML2_IMPORT_PATH", path.toUtf8());

        path = QStringLiteral("%1:%2").arg(INSTALL_DATADIR).arg(QString(qgetenv("XDG_DATA_DIRS")));
        qputenv("XDG_DATA_DIRS", path.toUtf8());

        path = QStringLiteral("%1:%2:/etc/xdg").arg(INSTALL_CONFIGDIR).arg(QString(qgetenv("XDG_CONFIG_DIRS")));
        qputenv("XDG_CONFIG_DIRS", path.toUtf8());

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

    // Set platform integration
    qputenv("SAL_USE_VCLPLUGIN", QByteArray("kde"));
    qputenv("QT_PLATFORM_PLUGIN", QByteArray("Hawaii"));
    qputenv("QT_QPA_PLATFORMTHEME", QByteArray("Hawaii"));
    qputenv("XDG_MENU_PREFIX", QByteArray("hawaii-"));
    qputenv("XDG_CURRENT_DESKTOP", QByteArray("Hawaii"));
    qputenv("XCURSOR_THEME", QByteArray("hawaii"));
    qputenv("XCURSOR_SIZE", QByteArray("16"));
}

bool SessionManager::registerDBus()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    // Start the D-Bus service
    (void)new SessionAdaptor(this);
    if (!bus.registerObject(objectPath, this)) {
        qCWarning(SESSION_MANAGER) << "Couldn't register /HawaiiSession D-Bus object:"
                                   << qPrintable(bus.lastError().message());
        return false;
    }
    if (!bus.registerService(interfaceName)) {
        qCWarning(SESSION_MANAGER) << "Couldn't register org.hawaii.session D-Bus service:"
                                   << qPrintable(bus.lastError().message());
        return false;
    }
    qCDebug(SESSION_MANAGER) << "Registered" << interfaceName << "D-Bus interface";

    // Register process launcher service
    if (!m_launcher->registerInterface())
        return false;

    // Register screen saver interface
    if (!m_screenSaver->registerInterface())
        return false;

    return true;
}

bool SessionManager::isLocked() const
{
    return m_locked;
}

void SessionManager::setLocked(bool value)
{
    m_locked = value;
}

void SessionManager::autostart()
{
    for (const XdgDesktopFile &entry: XdgAutoStart::desktopFileList()) {
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

    // Stop the compositor
    m_compositorLauncher->terminate();

    // Exit
    QCoreApplication::quit();
}

#include "moc_sessionmanager.cpp"
