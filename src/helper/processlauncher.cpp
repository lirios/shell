/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2019 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDebug>
#include <QDBusConnection>
#include <QDBusError>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>

#include <LiriXdg/DesktopFile>

#include "processlauncher.h"

const QString objectPath = QStringLiteral("/io/liri/Launcher");
const QString interfaceName = QStringLiteral("io.liri.Launcher");

ProcessLauncher::ProcessLauncher(QObject *parent)
    : QObject(parent)
{
    // Register D-Bus object
    auto bus = QDBusConnection::sessionBus();
    if (!bus.registerObject(objectPath, interfaceName,
                            this, QDBusConnection::ExportScriptableContents))
        qWarning("Failed to register \"%s\" D-Bus interface: %s", qPrintable(interfaceName),
                 qPrintable(bus.lastError().message()));
}

ProcessLauncher::~ProcessLauncher()
{
    // Unregister D-Bus object
    auto bus = QDBusConnection::sessionBus();
    bus.unregisterObject(objectPath);
}

bool ProcessLauncher::LaunchApplication(const QString &appId)
{
    if (appId.isEmpty())
        return false;

    const QString fileName = QStandardPaths::locate(
                QStandardPaths::ApplicationsLocation,
                appId + QStringLiteral(".desktop"));
    if (fileName.isEmpty()) {
        qWarning() << "Cannot find" << appId << "desktop file";
        return false;
    }

    Liri::DesktopFile *desktop = Liri::DesktopFileCache::getFile(fileName);
    if (!desktop) {
        qWarning() << "No desktop file found for" << appId;
        return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT"));
    desktop->setProcessEnvironment(env);

    return desktop->startDetached();
}

bool ProcessLauncher::LaunchDesktopFile(const QString &path, const QStringList &urls)
{
    if (path.isEmpty())
        return false;

    Liri::DesktopFile *desktop = nullptr;

    if (QFile::exists(path))
        desktop = new Liri::DesktopFile(path);
    else
        desktop = Liri::DesktopFileCache::getFile(path);

    if (!desktop) {
        qWarning() << "Failed to open desktop file" << path;
        return false;
    }

    auto env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT"));
    desktop->setProcessEnvironment(env);

    return desktop->startDetached(urls);
}

bool ProcessLauncher::LaunchCommand(const QString &command)
{
    if (command.isEmpty())
        return false;

    auto env = QProcessEnvironment::systemEnvironment();
    env.remove(QStringLiteral("QT_WAYLAND_USE_BYPASSWINDOWMANAGERHINT"));

    QProcess *process = new QProcess(this);
    process->setProcessEnvironment(env);
    return process->startDetached(command);
}
