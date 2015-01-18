/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileSystemWatcher>

#include "cmakedirs.h"
#include "processcontroller.h"

Q_LOGGING_CATEGORY(PROCESS_CONTROLLER, "hawaii.session.processcontroller")

#define FULLSCREEN_SHELL_SOCKET "hawaii-master-"
#define HAWAII_SOCKET "hawaii-slave-"

ProcessController::ProcessController(const QString &mode, QObject *parent)
    : QObject(parent)
    , m_fullScreenShell(Q_NULLPTR)
    , m_modeName(mode)
    , m_hasLibInputPlugin(false)
{
    if (mode == QStringLiteral("eglfs"))
        m_mode = EglFSMode;
    else if (mode == QStringLiteral("nested"))
        m_mode = NestedMode;
}

QString ProcessController::modeName() const
{
    return m_modeName;
}

ProcessController::Mode ProcessController::mode() const
{
    return m_mode;
}

QString ProcessController::compositorSocket() const
{
    return m_compositorSocket;
}

void ProcessController::start()
{
    // Prepare for execution
    prepare();

    // Actually start something
    if (m_fullScreenShell) {
        // Run the full screen shell compositor if enabled
        qCDebug(PROCESS_CONTROLLER)
                << "Running:" << qPrintable(m_fullScreenShell->program())
                << qPrintable(m_fullScreenShell->arguments().join(" "));
        m_fullScreenShell->start();

        if (!m_fullScreenShell->waitForStarted())
            qFatal("Full Screen Shell compositor cannot be started, aborting...");
    } else {
        // Start the compositor
        startCompositor();
    }
}

void ProcessController::stop()
{
    // Stop the compositor, this will also stop fullscreen shell
    // if it was started
    m_compositor->terminate();
    if (!m_compositor->waitForFinished())
        m_compositor->kill();
    m_compositor->deleteLater();
    m_compositor = Q_NULLPTR;
    Q_EMIT stopped();
}

QString ProcessController::randomString() const
{
    // Courtesy of Merlin069 from:
    // http://stackoverflow.com/questions/18862963/qt-c-random-string-generation

    const QString possibleCharacters("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
    const int randomStringLength = 12;

    QString randomString;
    for(int i = 0; i<randomStringLength; ++i) {
        int index = qrand() % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        randomString.append(nextChar);
    }

    return randomString;
}

void ProcessController::prepare()
{
    // Wayland sockets
    QString random = randomString();
    m_compositorSocket = QStringLiteral(HAWAII_SOCKET) + random;
    m_fullScreenShellSocket = QStringLiteral(FULLSCREEN_SHELL_SOCKET) + random;

    // Setup and print summary
    if (m_mode == NestedMode)
        setupFullScreenShell();
    setupCompositor();
    printSummary();
}

void ProcessController::setupFullScreenShell()
{
    // Full screen shell
    m_fullScreenShell = new QProcess(this);
    m_fullScreenShell->setProcessChannelMode(QProcess::ForwardedChannels);
    m_fullScreenShell->setProgram(QStringLiteral("weston"));
    m_fullScreenShell->setArguments(QStringList()
                                    << QStringLiteral("--shell=fullscreen-shell.so")
                                    << QStringLiteral("--socket=") + m_fullScreenShellSocket);
    connect(m_fullScreenShell, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(fullScreenShellFinished(int,QProcess::ExitStatus)));

    m_fullScreenShellWatcher = new QFileSystemWatcher(this);
    m_fullScreenShellWatcher->addPath(QString::fromUtf8(qgetenv("XDG_RUNTIME_DIR")));
    connect(m_fullScreenShellWatcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(directoryChanged(QString)));
}

void ProcessController::setupCompositor()
{
    // Do we have the libinput plugin? If available, it will be used in eglfs mode
    if (m_mode == EglFSMode) {
        for (const QString &path: QCoreApplication::libraryPaths()) {
            QDir pluginsDir(path + QStringLiteral("/generic"));
            for (const QString &fileName: pluginsDir.entryList(QDir::Files)) {
                if (fileName == QStringLiteral("libqlibinputplugin.so")) {
                    m_hasLibInputPlugin = true;
                    break;
                }
            }
        }
    }

    // Compositor process
    m_compositor = new QProcess(this);
    m_compositor->setProcessChannelMode(QProcess::ForwardedChannels);
    m_compositor->setProgram(QStringLiteral(INSTALL_BINDIR "/hawaii"));
    m_compositor->setArguments(QStringList()
                               << QStringLiteral("-p")
                               << QStringLiteral("org.hawaii.desktop"));
    if (m_mode == NestedMode) {
        m_compositor->setArguments(m_compositor->arguments()
                                   << QStringLiteral("-platform")
                                   << QStringLiteral("wayland")
                                   << QStringLiteral("--socket=") + m_compositorSocket);
    } else if (m_mode == EglFSMode) {
        m_compositor->setArguments(m_compositor->arguments()
                                   << QStringLiteral("-platform")
                                   << QStringLiteral("eglfs"));
        if (m_hasLibInputPlugin) {
            m_compositor->setArguments(m_compositor->arguments()
                                       << QStringLiteral("-plugin")
                                       << QStringLiteral("libinput"));
        }
    }
    connect(m_compositor, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_compositor, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(compositorFinished(int,QProcess::ExitStatus)));

    // Pass arguments for full screen shell and style
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (m_fullScreenShell)
        env.insert(QStringLiteral("WAYLAND_DISPLAY"), m_fullScreenShellSocket);
    env.insert(QStringLiteral("QT_QUICK_CONTROLS_STYLE"), QStringLiteral("Wind"));
    env.insert(QStringLiteral("XCURSOR_THEME"), QStringLiteral("hawaii"));
    env.insert(QStringLiteral("XCURSOR_SIZE"), QStringLiteral("16"));
    if (qEnvironmentVariableIsSet("DISPLAY") && !m_fullScreenShell)
        env.insert(QStringLiteral("QT_XCB_GL_INTEGRATION"), QStringLiteral("xcb_egl"));
    if (m_mode == EglFSMode && m_hasLibInputPlugin)
        env.insert(QStringLiteral("QT_QPA_EGLFS_DISABLE_INPUT"), QStringLiteral("1"));
    m_compositor->setProcessEnvironment(env);
}

void ProcessController::printSummary()
{
    qCDebug(PROCESS_CONTROLLER) << "Mode:" << m_modeName;
    switch (m_mode) {
    case EglFSMode:
        qCDebug(PROCESS_CONTROLLER) << "libinput:" << m_hasLibInputPlugin;
        break;
    case NestedMode:
        qCDebug(PROCESS_CONTROLLER) << "Weston socket:" << m_fullScreenShellSocket;
        qCDebug(PROCESS_CONTROLLER) << "Compositor socket:" << m_compositorSocket;
        break;
    default:
        break;
    }
    qCDebug(PROCESS_CONTROLLER) << "X11:" << qEnvironmentVariableIsSet("DISPLAY");
}

void ProcessController::startCompositor()
{
    // Start the process
    qDebug(PROCESS_CONTROLLER)
            << "Running:" << qPrintable(m_compositor->program())
            << qPrintable(m_compositor->arguments().join(" "));
    m_compositor->start();
    if (!m_compositor->waitForStarted()) {
        // Compositor failed to start, kill full screen shell and terminate
        qFatal("Compositor won't start, aborting...");
        compositorFinished(0, QProcess::NormalExit);
    }
}

void ProcessController::directoryChanged(const QString &path)
{
    Q_UNUSED(path);
    Q_ASSERT(m_fullScreenShell);

    // Don't start until the full screen shell compositor is up
    const QString socketFileName = path + QStringLiteral("/") +
            m_fullScreenShellSocket;
    if (!QFile::exists(socketFileName))
        return;

    // Socket is here, let's bring the compositor up but first disconnect
    // and destroy the file system watcher otherwise this slot gets
    // called over and over again
    m_fullScreenShellWatcher->disconnect(this);
    m_fullScreenShellWatcher->deleteLater();
    m_fullScreenShellWatcher = Q_NULLPTR;
    startCompositor();
}

void ProcessController::compositorFinished(int code, const QProcess::ExitStatus &status)
{
    Q_UNUSED(status);

    if (code != 0)
        qCWarning(PROCESS_CONTROLLER) << "Compositor finished with exit code" << code;

    // Whathever the reason why it finished is we need to quit the
    // full screen shell compositor, if any
    if (m_fullScreenShell) {
        m_fullScreenShell->terminate();
        if (!m_fullScreenShell->waitForFinished())
            m_fullScreenShell->kill();
        m_fullScreenShell->deleteLater();
        m_fullScreenShell = Q_NULLPTR;
    }
}

void ProcessController::fullScreenShellFinished(int code, const QProcess::ExitStatus &status)
{
    Q_UNUSED(status);

    if (code != 0)
        qCWarning(PROCESS_CONTROLLER) << "Full screen shell finished with exit code" << code;

    // Kill the compositor if for some reason is still running
    m_compositor->terminate();
    if (!m_compositor->waitForFinished())
        m_compositor->kill();
}
