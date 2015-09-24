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
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>

#include <GreenIsland/Compositor>

#include <qt5xdg/xdgdesktopfile.h>

#include "processlauncher.h"

Q_LOGGING_CATEGORY(LAUNCHER, "hawaii.launcher")

using namespace GreenIsland;

ProcessLauncher::ProcessLauncher(QObject *parent)
    : QObject(parent)
{
}

ProcessLauncher::~ProcessLauncher()
{
    closeApplications();
}

void ProcessLauncher::closeApplications()
{
    qCDebug(LAUNCHER) << "Terminate applications";

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

bool ProcessLauncher::closeApplication(const QString &appId)
{
    const QString fileName = appId + QStringLiteral(".desktop");
    return closeEntry(fileName);
}

bool ProcessLauncher::closeDesktopFile(const QString &fileName)
{
    qCDebug(LAUNCHER) << "Closing application for" << fileName;
    return closeEntry(fileName);
}

bool ProcessLauncher::launchEntry(XdgDesktopFile *entry)
{
    QStringList args = entry->expandExecString();
    QString command = args.takeAt(0);

    qCDebug(LAUNCHER) << "Launching" << entry->expandExecString().join(" ") << "from" << entry->fileName();

    const QString waylandDisplay = Compositor::instance()->socketName();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!waylandDisplay.isEmpty())
        env.insert(QStringLiteral("WAYLAND_DISPLAY"), waylandDisplay);
    env.insert(QStringLiteral("SAL_USE_VCLPLUGIN"), QStringLiteral("kde"));
    env.insert(QStringLiteral("QT_PLATFORM_PLUGIN"), QStringLiteral("Hawaii"));
    env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("wayland"));
    env.insert(QStringLiteral("QT_QPA_PLATFORMTHEME"), QStringLiteral("Hawaii"));
    env.insert(QStringLiteral("QT_QUICK_CONTROLS_STYLE"), QStringLiteral("Aluminium"));
    env.insert(QStringLiteral("XCURSOR_THEME"), QStringLiteral("hawaii"));
    env.insert(QStringLiteral("XCURSOR_SIZE"), QStringLiteral("16"));
    env.remove(QStringLiteral("QSG_RENDER_LOOP"));

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

bool ProcessLauncher::closeEntry(const QString &fileName)
{
    if (!m_apps.contains(fileName))
        return false;

    QProcess *process = m_apps[fileName];
    process->terminate();
    if (!process->waitForFinished())
        process->kill();
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
        qCDebug(LAUNCHER) << "Application for" << fileName << "finished with exit code" << exitCode;
        m_apps.remove(fileName);
        process->deleteLater();
    }
}

#include "moc_processlauncher.cpp"
