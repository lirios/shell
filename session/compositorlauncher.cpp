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
#include <QtCore/QDir>
#include <QtCore/QFile>

#include "cmakedirs.h"
#include "compositorlauncher.h"

Q_LOGGING_CATEGORY(COMPOSITOR, "hawaii.session.compositor")

Q_GLOBAL_STATIC(CompositorLauncher, s_compositorLauncher)

CompositorLauncher::CompositorLauncher(QObject *parent)
    : QObject(parent)
    , m_mode(UnknownMode)
    , m_hardware(UnknownHardware)
    , m_sessionSocketName(QStringLiteral("hawaii-session"))
    , m_hasLibInputPlugin(false)
    , m_weston(new CompositorProcess(false, this))
    , m_compositor(new CompositorProcess(false, this))
{
    // Starts the compositor as soon as Weston is started
    connect(m_weston, &CompositorProcess::started,
            m_compositor, &CompositorProcess::start);

    // Map compositor started/finished to us
    connect(m_weston, &CompositorProcess::finished,
            this, &CompositorLauncher::finished);
    connect(m_compositor, &CompositorProcess::started,
            this, &CompositorLauncher::started);
    connect(m_compositor, &CompositorProcess::stopped,
            this, &CompositorLauncher::finished);
    connect(m_compositor, &CompositorProcess::finished,
            this, &CompositorLauncher::finished);
}

CompositorLauncher *CompositorLauncher::instance()
{
    return s_compositorLauncher();
}

CompositorLauncher::Mode CompositorLauncher::mode() const
{
    return m_mode;
}

void CompositorLauncher::setMode(const Mode &mode)
{
    m_mode = mode;
}

QString CompositorLauncher::socketName() const
{
    return m_socketName;
}

QString CompositorLauncher::sessionSocketName() const
{
    return m_sessionSocketName;
}

void CompositorLauncher::start()
{
    // Try to detect mode and hardware
    detectHardware();
    detectMode();
    if (m_mode == UnknownMode) {
        qCWarning(COMPOSITOR) << "No mode detected, please manually specify one!";
        QCoreApplication::quit();
        return;
    }

    // Detect whether we have libinput
    detectLibInput();

    // Setup the sockets
    if (m_mode == NestedMode) {
        m_socketName = QStringLiteral("hawaii-slave");
        m_weston->setSocketName(QStringLiteral("hawaii-master"));
        m_compositor->setSocketName(m_socketName);

        // Weston must be session leader when we are launched
        // from gdm
        //if (qEnvironmentVariableIsSet("GDMSESSION"))
            //m_weston->setSessionLeader(true);

        m_weston->setProgram(QStringLiteral("weston"));
        m_weston->setArguments(QStringList()
                               << QStringLiteral("--shell=fullscreen-shell.so")
                               << QStringLiteral("--socket=%1").arg(m_weston->socketName()));
    } else {
        m_socketName = QStringLiteral("hawaii");
        m_compositor->setSocketName(m_socketName);
    }

    // Setup the process
    m_compositor->setProgram(QStringLiteral(INSTALL_BINDIR "/hawaii"));
    m_compositor->setArguments(compositorArgs());
    m_compositor->setEnvironment(compositorEnv());

    // Summary
    printSummary();

    // Start the process
    if (m_mode == NestedMode)
        m_weston->start();
    else
        m_compositor->start();

    // Set environment so that applications will inherit it
    setupEnvironment();
}

void CompositorLauncher::stop()
{
    m_compositor->stop();
    if (m_mode == NestedMode)
        m_weston->stop();
}

void CompositorLauncher::terminate()
{
    // We want to terminate both ourselves
    disconnect(m_compositor, &CompositorProcess::finished,
               m_weston, &CompositorProcess::stop);

    m_compositor->terminate();
    if (m_mode == NestedMode)
        m_weston->terminate();
}

void CompositorLauncher::detectMode()
{
    // Can't detect mode when it was forced by arguments
    if (m_mode != UnknownMode)
        return;

    // Detect Wayland
    if (qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
        m_mode = WaylandMode;
        return;
    }

    // Detect X11
    if (qEnvironmentVariableIsSet("DISPLAY")) {
        m_mode = X11Mode;
        return;
    }

    // Detect eglfs since Qt 5.5
    if (qEnvironmentVariableIsSet("QT_QPA_EGLFS_INTEGRATION")) {
        m_mode = EglFSMode;
        return;
    }

    // Use eglfs mode if we detected a particular hardware
    if (m_hardware != UnknownHardware) {
        m_mode = EglFSMode;
        return;
    }

    // Detect drm
    if (QDir(QStringLiteral("/sys/class/drm")).exists()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        m_mode = EglFSMode;
#else
        m_mode = NestedMode;
#endif
        return;
    }

    // TODO: Detect hwcomposer

    // Default to nested mode
    m_mode = NestedMode;
}

void CompositorLauncher::detectHardware()
{
    // Detect Broadcom
    bool found = deviceModel().startsWith(QStringLiteral("Raspberry"));
    if (!found) {
        QStringList paths = QStringList()
                << QStringLiteral("/usr/bin/vcgencmd")
                << QStringLiteral("/opt/vc/bin/vcgencmd")
                << QStringLiteral("/proc/vc-cma");
        found = pathsExist(paths);
    }
    if (found) {
        m_hardware = BroadcomHardware;
        return;
    }

    // TODO: Detect Mali
    // TODO: Detect Vivante

    // Unknown hardware
    m_hardware = UnknownHardware;
}

QString CompositorLauncher::deviceModel() const
{
    QFile file(QStringLiteral("/proc/device-tree/model"));
    if (file.open(QIODevice::ReadOnly)) {
        QString data = QString::fromUtf8(file.readAll());
        file.close();
        return data;
    }

    return QString();
}

bool CompositorLauncher::pathsExist(const QStringList &paths) const
{
    Q_FOREACH (const QString &path, paths) {
        if (QFile::exists(path))
            return true;
    }

    return false;
}

void CompositorLauncher::detectLibInput()
{
    // Do we have the libinput plugin?
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

QStringList CompositorLauncher::compositorArgs() const
{
    QStringList args;

    // Standard arguments
    args << QStringLiteral("-platformtheme") << QStringLiteral("Hawaii");
    args << QStringLiteral("--session-socket=%1").arg(m_sessionSocketName);

    // Specific arguments
    switch (m_mode) {
    case EglFSMode:
    case HwComposerMode:
        if (m_hasLibInputPlugin)
            args << QStringLiteral("-plugin") << QStringLiteral("libinput");
        else
            args << QStringLiteral("-plugin") << QStringLiteral("EvdevMouse")
                 << QStringLiteral("-plugin") << QStringLiteral("EvdevKeyboard");
        break;
    case NestedMode:
        args << QStringLiteral("--socket=%1").arg(m_compositor->socketName());
        break;
    default:
        break;
    }

    return args;
}

QProcessEnvironment CompositorLauncher::compositorEnv() const
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // Standard environment
    env.insert(QStringLiteral("QT_QPA_PLATFORMTHEME"), QStringLiteral("Hawaii"));
    env.insert(QStringLiteral("QT_QUICK_CONTROLS_STYLE"), QStringLiteral("Wind"));
    env.insert(QStringLiteral("XCURSOR_THEME"), QStringLiteral("hawaii"));
    env.insert(QStringLiteral("XCURSOR_SIZE"), QStringLiteral("16"));
    env.insert(QStringLiteral("QSG_RENDER_LOOP"), QStringLiteral("windows"));

    // Specific environment
    switch (m_mode) {
    case NestedMode:
        env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("wayland"));
        env.insert(QStringLiteral("WAYLAND_DISPLAY"), m_weston->socketName());

        if (m_hardware == BroadcomHardware) {
            env.insert(QStringLiteral("QT_WAYLAND_HARDWARE_INTEGRATION"), QStringLiteral("brcm-egl"));
            env.insert(QStringLiteral("QT_WAYLAND_CLIENT_BUFFER_INTEGRATION"), QStringLiteral("brcm-egl"));
        }
        break;
    case EglFSMode:
        // General purpose distributions do not have the proper eglfs
        // integration and may want to build it out of tree, let them
        // specify a QPA plugin with an environment variable
        if (qEnvironmentVariableIsSet("HAWAII_QPA_PLATFORM"))
            env.insert(QStringLiteral("QT_QPA_PLATFORM"), qgetenv("HAWAII_QPA_PLATFORM"));
        else
            env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("eglfs"));

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        switch (m_hardware) {
        case BroadcomHardware:
            env.insert(QStringLiteral("QT_QPA_EGLFS_INTEGRATION"), QStringLiteral("eglfs_brcm"));
            break;
        default:
            env.insert(QStringLiteral("QT_QPA_EGLFS_INTEGRATION"), QStringLiteral("eglfs_kms"));
            break;
        }
#endif

        if (m_hasLibInputPlugin)
            env.insert(QStringLiteral("QT_QPA_EGLFS_DISABLE_INPUT"), QStringLiteral("1"));
        break;
    case HwComposerMode:
        env.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("hwcomposer"));
        env.insert(QStringLiteral("EGL_PLATFORM"), QStringLiteral("hwcomposer"));
        env.insert(QStringLiteral("QT_COMPOSITOR_NEGATE_INVERTED_Y"), QStringLiteral("0"));
        env.insert(QStringLiteral("QT_QPA_EGLFS_DEPTH"), QStringLiteral("32"));
        env.insert(QStringLiteral("QT_QPA_EGLFS_HIDECURSOR"), QStringLiteral("1"));
        if (m_hasLibInputPlugin)
            env.insert(QStringLiteral("QT_QPA_EGLFS_DISABLE_INPUT"), QStringLiteral("1"));
        break;
    case X11Mode:
        env.insert(QStringLiteral("QT_XCB_GL_INTEGRATION"), QStringLiteral("xcb_egl"));
        break;
    default:
        break;
    }

    return env;
}

void CompositorLauncher::printSummary()
{
    auto modeToString = [this] {
        switch (m_mode) {
        case EglFSMode:
            return QStringLiteral("eglfs");
        case HwComposerMode:
            return QStringLiteral("hwcomposer");
        case NestedMode:
            return QStringLiteral("nested");
        case X11Mode:
            return QStringLiteral("x11");
        case WaylandMode:
            return QStringLiteral("wayland");
        default:
            return QStringLiteral("unknown");
        }
    };

    auto hwToString = [this] {
        switch (m_hardware) {
        case DrmHardware:
            return QStringLiteral("drm");
        case BroadcomHardware:
            return QStringLiteral("broadcom");
        default:
            return QStringLiteral("unknown");
        }
    };

    qCDebug(COMPOSITOR) << "Mode:" << modeToString();
    qCDebug(COMPOSITOR) << "Hardware:" << hwToString();
    qCDebug(COMPOSITOR) << "libinput:" << m_hasLibInputPlugin;
    if (m_mode == NestedMode)
        qCDebug(COMPOSITOR) << "Weston socket:" << m_weston->socketName();
    qCDebug(COMPOSITOR) << "Compositor socket:" << m_compositor->socketName();
    if (m_mode == X11Mode)
        qCDebug(COMPOSITOR) << "X11 display:" << qgetenv("DISPLAY");
    Q_FOREACH (const QString &key, m_compositor->environment().keys()) {
        if (key.startsWith(QStringLiteral("QT_")))
            qCDebug(COMPOSITOR, "%s=%s", qPrintable(key), qPrintable(m_compositor->environment().value(key)));
    }
}

void CompositorLauncher::setupEnvironment()
{
    // Make clients run on Wayland
    if (m_hardware == BroadcomHardware) {
        qputenv("QT_QPA_PLATFORM", QByteArray("wayland-brcm"));
        qputenv("QT_WAYLAND_HARDWARE_INTEGRATION", QByteArray("brcm-egl"));
        qputenv("QT_WAYLAND_CLIENT_BUFFER_INTEGRATION", QByteArray("brcm-egl"));
    } else {
        qputenv("QT_QPA_PLATFORM", QByteArray("wayland"));
    }
    qputenv("GDK_BACKEND", QByteArray("wayland"));

    // Set WAYLAND_DISPLAY only when nested, otherwise we don't need to do
    // it because applications can detect the socket themselves
    if (m_mode == NestedMode)
        qputenv("WAYLAND_DISPLAY", m_compositor->socketName().toLatin1());
    else
        qunsetenv("WAYLAND_DISPLAY");
}

#include "moc_compositorlauncher.cpp"
