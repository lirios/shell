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

#include "libqtxdg/xdgautostart.h"
#include "libqtxdg/xdgdesktopfile.h"

#include "cmakedirs.h"
#include "processcontroller.h"
#include "sessionmanager.h"

#include <sys/types.h>
#include <signal.h>

Q_LOGGING_CATEGORY(SESSION_MANAGER, "hawaii.session.manager")

SessionManager::SessionManager(ProcessController *controller)
    : QObject(controller)
    , m_controller(controller)
{
    // Autostart applications as soon as the compositor is ready
    connect(m_controller, &ProcessController::started, [=]() {
        // Prepare the environment to run applications into our compositor
        qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
        qputenv("GDK_BACKEND", QByteArray("wayland"));

        // Set WAYLAND_DISPLAY only when nested, otherwise we don't need to do
        // it because applications can detect the socket themselves
        if (m_controller->mode() == QStringLiteral("nested"))
            qputenv("WAYLAND_DISPLAY", qPrintable(m_controller->compositorSocket()));

        // Autostart
        autostart();
    });

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

void SessionManager::autostart()
{
    for (const XdgDesktopFile &entry: XdgAutoStart::desktopFileList()) {
        if (!entry.isSuitable(true, QStringLiteral("X-Hawaii")))
            continue;

        QStringList args = entry.expandExecString();
        QString command = args.takeAt(0);

        qCDebug(SESSION_MANAGER) << "Starting" << entry.expandExecString().join(" ") << "from" << entry.fileName();

        qint64 pid = 0;
        if (QProcess::startDetached(command, args, QString(), &pid)) {
            qCDebug(SESSION_MANAGER,
                    "Started \"%s\" (%s) automatically with pid %lld",
                    qPrintable(entry.fileName()),
                    qPrintable(entry.name()),
                    pid);
            m_processes.append(pid);
        } else {
            qCWarning(SESSION_MANAGER,
                      "Failed to start \"%s\" (%s) automatically",
                      qPrintable(entry.fileName()),
                      qPrintable(entry.name()));
        }
    }
}

void SessionManager::logOut()
{
    // Kill the autostart programs
    for (qint64 pid: m_processes) {
        if (kill(pid, SIGTERM) < 0)
            kill(pid, SIGKILL);
    }

    // Stop the compositor
    m_controller->stop();
}

#include "moc_sessionmanager.cpp"
