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

#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>

#include "libqtxdg/xdgdesktopfile.h"

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include "processcontroller.h"
#include "processlauncher.h"
#include "sessionmanager.h"

Q_LOGGING_CATEGORY(LAUNCHER, "hawaii.session.launcher")

ProcessLauncher::ProcessLauncher(SessionManager *sessionManager)
    : QDBusAbstractAdaptor(sessionManager)
    , m_sessionManager(sessionManager)
{
}

ProcessLauncher::~ProcessLauncher()
{
    // Terminate all process launched by us
    ApplicationMapIterator i(m_apps);
    while (i.hasNext()) {
        i.next();

        QString fileName = i.key();
        QProcess *process = i.value();

        i.remove();

        qCDebug(LAUNCHER) << "Terminating application from" << fileName << "with pid" << process->pid();

        process->terminate();
        if (!process->waitForFinished())
            process->kill();
        process->deleteLater();
    }
}

bool ProcessLauncher::registerInterface()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.registerObject(objectPath, this)) {
        qCWarning(LAUNCHER,
                  "Couldn't register %s D-Bus object: %s",
                  objectPath,
                  qPrintable(bus.lastError().message()));
        return false;
    }

    qCDebug(LAUNCHER) << "Registered" << interfaceName << "D-Bus interface";
    return true;
}

bool ProcessLauncher::launchApplication(const QString &appId)
{
    const QString fileName =
            QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                                   appId + QStringLiteral(".desktop"));
    XdgDesktopFile *entry = XdgDesktopFileCache::getFile(fileName);
    if (!entry) {
        qCWarning(LAUNCHER) << "No desktop entry found for" << appId;
        return false;
    }

    return launchEntry(entry);
}

bool ProcessLauncher::launchDesktopFile(const QString &fileName)
{
    XdgDesktopFile *entry = XdgDesktopFileCache::getFile(fileName);
    if (!entry) {
        qCWarning(LAUNCHER) << "Failed to open desktop file" << fileName;
        return false;
    }

    return launchEntry(entry);
}

bool ProcessLauncher::launchEntry(XdgDesktopFile *entry)
{
    QStringList args = entry->expandExecString();
    QString command = args.takeAt(0);

    qCDebug(LAUNCHER) << "Launching" << entry->expandExecString().join(" ") << "from" << entry->fileName();

    // Prepare the environment to run applications into our compositor
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    for (const QString &key: env.keys()) {
        if (key == QStringLiteral("QT_QPA_PLATFORMTHEME"))
            continue;
        if (key.startsWith(QStringLiteral("QT_QPA")) || key.startsWith(QStringLiteral("EGL")))
            env.remove(key);
    }
    env.insert(QStringLiteral("QT_WAYLAND_USE_XDG_SHELL"), QStringLiteral("1"));
    env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("wayland"));
    env.insert(QStringLiteral("GDK_BACKEND"), QStringLiteral("wayland"));

    // Set WAYLAND_DISPLAY only when nested, otherwise we don't need to do
    // it because applications can detect the socket themselves
    if (m_sessionManager->processController()->mode() == ProcessController::NestedMode)
        env.insert(QStringLiteral("WAYLAND_DISPLAY"), m_sessionManager->processController()->compositorSocket());

    QProcess *process = new QProcess(this);
    process->setProgram(command);
    process->setArguments(args);
    process->setProcessEnvironment(env);
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    m_apps[entry->fileName()] = process;
    connect(process, SIGNAL(finished(int)), this, SLOT(finished(int)));
    process->start();
    if (!process->waitForStarted()) {
        qCWarning(LAUNCHER,
                  "Failed to launch \"%s\" (%s)",
                  qPrintable(entry->fileName()),
                  qPrintable(entry->name()));
        return false;
    }

    qCDebug(LAUNCHER,
            "Launched \"%s\" (%s) with pid %lld",
            qPrintable(entry->fileName()),
            qPrintable(entry->name()),
            process->pid());

    return true;
}

void ProcessLauncher::finished(int exitCode)
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (!process)
        return;

    QString fileName = m_apps.key(process);
    XdgDesktopFile *entry = XdgDesktopFileCache::getFile(fileName);
    if (entry) {
        qCDebug(LAUNCHER) << "Application" << entry->name() << "finished with exit code" << exitCode;

        m_apps.remove(fileName);
        process->deleteLater();
    }
}

#include "moc_processlauncher.cpp"
