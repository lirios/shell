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

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include "libqtxdg/xdgautostart.h"
#include "libqtxdg/xdgdesktopfile.h"

#include "cmakedirs.h"
#include "processcontroller.h"
#include "processlauncher.h"
#include "sessionadaptor.h"
#include "sessionmanager.h"

#include <sys/types.h>
#include <signal.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "hawaii.session.manager")

SessionManager::SessionManager(ProcessController *controller)
    : QObject(controller)
    , m_controller(controller)
    , m_launcher(new ProcessLauncher(this))
{
    // Autostart applications as soon as the compositor is ready
    connect(m_controller, &ProcessController::started,
            this, &SessionManager::autostart);

    // The compositor is stopped when a logout has been request,
    // which happens after we killed all the processes so when this
    // arrives we can emit the loggedOut signal
    connect(m_controller, SIGNAL(stopped()), this, SIGNAL(loggedOut()));
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

    return true;
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
    // Stop the compositor
    m_controller->stop();
}

#include "moc_sessionmanager.cpp"
