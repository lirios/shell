/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtCore/QVariantMap>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/QWaylandInputDevice>
#include <QtCompositor/private/qwlcompositor_p.h>
#include <QtCompositor/private/qwlinputdevice_p.h>
#include <QtCompositor/private/qwlpointer_p.h>

#include "cmakedirs.h"
#include "compositor.h"
#include "panelmanager.h"
#include "shell.h"
#include "shellsurface.h"
#include "clientwindow.h"
#include "workspace.h"
#include "notifications.h"
#include "screensaver.h"
#include "keymap.h"

/*
 * CompositorPrivate
 */

class CompositorPrivate
{
public:
    CompositorPrivate(Compositor *self);

    void closeShell();
    void dpms(bool on);

    void _q_shellStarted();
    void _q_shellFailed(QProcess::ProcessError error);
    void _q_shellReadyReadStandardOutput();
    void _q_shellReadyReadStandardError();
    void _q_shellAboutToClose();

    QString shellFileName;
    QProcess *shellProcess;
    Compositor::State state;

protected:
    Q_DECLARE_PUBLIC(Compositor)
    Compositor *const q_ptr;
};

CompositorPrivate::CompositorPrivate(Compositor *self)
    : shellProcess(nullptr)
    , state(Compositor::Active)
    , q_ptr(self)
{
    shellFileName = QStringLiteral(INSTALL_LIBEXECDIR "/starthawaii");
}

void CompositorPrivate::closeShell()
{
    if (!shellProcess)
        return;

    shellProcess->close();
    delete shellProcess;
    shellProcess = nullptr;

    Q_Q(Compositor);
    Q_EMIT q->shellClientRunningChanged();
}

void CompositorPrivate::dpms(bool on)
{
    // TODO
}
void CompositorPrivate::_q_shellStarted()
{
    Q_Q(Compositor);
    Q_EMIT q->shellClientRunningChanged();
}

void CompositorPrivate::_q_shellFailed(QProcess::ProcessError error)
{
    Q_Q(Compositor);

    switch (error) {
    case QProcess::FailedToStart:
        qWarning()
                << "The shell process failed to start.\n"
                << "Either the invoked program is missing, or you may have insufficient permissions to run it.";
        break;
    case QProcess::Crashed:
        qWarning()
                << "The shell process crashed some time after starting successfully.";
        break;
    case QProcess::Timedout:
        qWarning()
                << "The shell process timedout.\n";
        break;
    case QProcess::WriteError:
        qWarning()
                << "An error occurred when attempting to write to the shell process.";
        break;
    case QProcess::ReadError:
        qWarning()
                << "An error occurred when attempting to read from the shell process.";
        break;
    case QProcess::UnknownError:
        qWarning()
                << "Unknown error starting the shell process!";
        break;
    }

    // Don't need it anymore because it failed
    shellProcess->close();
    delete shellProcess;
    shellProcess = nullptr;
    Q_EMIT q->shellClientRunningChanged();
}

void CompositorPrivate::_q_shellReadyReadStandardOutput()
{
    if (shellProcess)
        qDebug() << qPrintable(shellProcess->readAllStandardOutput().trimmed());
}

void CompositorPrivate::_q_shellReadyReadStandardError()
{
    if (shellProcess)
        qDebug() << qPrintable(shellProcess->readAllStandardError().trimmed());
}

void CompositorPrivate::_q_shellAboutToClose()
{
    qDebug() << "Shell is about to close...";
}

/*
 * Compositor
 */

Compositor::Compositor(QWindow *window)
    : QWaylandCompositor(window)
    , d_ptr(new CompositorPrivate(this))
    , m_shellReady(false)
    , m_cursorSurface(nullptr)
    , m_cursorHotspotX(0)
    , m_cursorHotspotY(0)
{
    // Create interfaces
    m_notifications = new Notifications(waylandDisplay());
    m_screenSaver = new ScreenSaver(waylandDisplay());
    m_screenSaver->setCompositor(this);
    m_panelManager = new PanelManager(waylandDisplay());
    m_shellSurface = new ShellSurface(waylandDisplay());
    m_shell = new Shell(waylandDisplay());
    m_shell->setCompositor(this);
    connect(m_shell, &Shell::ready, [=]() {
        // Shell is ready and we can start handling input events
        m_shellReady = true;

        // Fade in the desktop
        Q_EMIT ready();
    });
    connect(m_shell, &Shell::lockedChanged, [=](bool value) {
        if (value)
            Q_EMIT locked();
        else
            Q_EMIT unlocked();
    });

    // Connect to signals
    connect(this, SIGNAL(sceneGraphInitialized()),
            this, SLOT(sceneGraphInitialized()),
            Qt::DirectConnection);
}

Compositor::~Compositor()
{
    stopShell();

    qDeleteAll(m_clientWindows);
    qDeleteAll(m_workspaces);
    delete m_screenSaver;
    delete m_notifications;
    delete m_panelManager;
    delete m_shellSurface;
    delete m_shell;
    delete d_ptr;
}

QString Compositor::shellFileName() const
{
    Q_D(const Compositor);
    return d->shellFileName;
}

void Compositor::setShellFileName(const QString &fileName)
{
    Q_D(Compositor);

    if (d->shellFileName != fileName) {
        d->shellFileName = fileName;
        Q_EMIT shellFileNameChanged();
    }
}

bool Compositor::isShellClientRunning() const
{
    Q_D(const Compositor);
    return (d->shellProcess != nullptr);
}

Compositor::State Compositor::state() const
{
    Q_D(const Compositor);
    return d->state;
}

void Compositor::setState(Compositor::State state)
{
    Q_D(Compositor);

    if (state == Compositor::Active && d->state == state) {
        Q_EMIT idleInhibitResetRequested();
        Q_EMIT idleTimerStartRequested();
        return;
    }

    if (d->state != state) {
        switch (state) {
        case Compositor::Active:
            switch (d->state) {
            case Compositor::Sleeping:
                d->dpms(true);
            default:
                Q_EMIT wake();
                Q_EMIT idleInhibitResetRequested();
                Q_EMIT idleTimerStartRequested();
            }
        case Compositor::Idle:
            Q_EMIT idle();
            Q_EMIT idleInhibitResetRequested();
            Q_EMIT idleTimerStopRequested();
            break;
        case Compositor::Offscreen:
            switch (d->state) {
            case Compositor::Sleeping:
                d->dpms(true);
            default:
                Q_EMIT idleInhibitResetRequested();
                Q_EMIT idleTimerStopRequested();
            }
        case Compositor::Sleeping:
            Q_EMIT idleInhibitResetRequested();
            Q_EMIT idleTimerStopRequested();
            d->dpms(false);
            break;
        }

        d->state = state;
        Q_EMIT stateChanged();
    }
}

Shell *Compositor::shell() const
{
    return m_shell;
}

ShellSurface *Compositor::shellSurface() const
{
    return m_shellSurface;
}

ScreenSaver *Compositor::screenSaver() const
{
    return m_screenSaver;
}

void Compositor::surfaceCreated(QWaylandSurface *surface)
{
    if (!surface || !surface->hasShellSurface())
        return;

    // Set window role
    surface->setWindowProperty(QStringLiteral("role"), Compositor::ApplicationRole);

    // Create application window instance
    ClientWindow *appWindow = new ClientWindow(waylandDisplay());
    appWindow->setSurface(surface);
    m_clientWindows.append(appWindow);

    // Delete application window on surface destruction
    connect(surface, &QWaylandSurface::destroyed, [=](QObject *object = 0) {
        for (ClientWindow *appWindow: m_clientWindows) {
            if (appWindow->surface() == surface) {
                if (m_clientWindows.removeOne(appWindow))
                    delete appWindow;
                break;
            }
        }
    });
}

QPointF Compositor::calculateInitialPosition(QWaylandSurface *surface)
{
    // As a heuristic place the new window on the same output as the
    // pointer. Falling back to the output containing 0,0.
    // TODO: Do something clever for touch too
    QPointF pos = defaultInputDevice()->handle()->pointerDevice()->currentPosition();

    // Find the target output (the one where the coordinates are in)
    // FIXME: QtWayland::Compositor should give us a list of outputs and then we
    // can scroll the list and find the output that contains the coordinates;
    // targetOutput then would be a pointer to an Output
    QtWayland::Compositor *compositor = static_cast<QWaylandCompositor *>(this)->handle();
    bool targetOutput = compositor->outputGeometry().contains(pos.toPoint());

    // Just move the surface to a random position if we can't find a target output
    if (!targetOutput) {
        pos.setX(10 + qrand() % 400);
        pos.setY(10 + qrand() % 400);
        return pos;
    }

    // Valid range within output where the surface will still be onscreen.
    // If this is negative it means that the surface is bigger than
    // output in this case we fallback to 0,0 in available geometry space.
    // FIXME: When we have an Output we need a way to check the available
    // geometry (which is the portion of screen that is not used by panels)
    int rangeX = compositor->outputGeometry().width() - surface->size().width();
    int rangeY = compositor->outputGeometry().height() - surface->size().height();

    int dx = 0, dy = 0;
    if (rangeX > 0)
        dx = qrand() % rangeX;
    if (rangeY > 0)
        dy = qrand() % rangeY;

    // Set surface position
    pos.setX(compositor->outputGeometry().x() + dx);
    pos.setY(compositor->outputGeometry().y() + dy);

    return pos;
}

void Compositor::startShell()
{
    Q_D(Compositor);

    // Sanity check
    if (d->shellFileName.isEmpty() || d->shellProcess)
        return;

    // Run the shell client process
    d->shellProcess = new QProcess(this);
    connect(d->shellProcess, SIGNAL(started()),
            this, SLOT(_q_shellStarted()));
    connect(d->shellProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(_q_shellFailed(QProcess::ProcessError)));
    connect(d->shellProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(_q_shellReadyReadStandardOutput()));
    connect(d->shellProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(_q_shellReadyReadStandardError()));
    connect(d->shellProcess, SIGNAL(aboutToClose()),
            this, SLOT(_q_shellAboutToClose()));
    d->shellProcess->start(d->shellFileName, QStringList(), QIODevice::ReadOnly);
}

void Compositor::stopShell()
{
    Q_D(Compositor);
    d->closeShell();
}

void Compositor::lockSession()
{
    m_shell->lockSession();
}

void Compositor::unlockSession()
{
    m_shell->unlockSession();
}

void Compositor::sceneGraphInitialized()
{
    //showGraphicsInfo();
}

#if 0
void Compositor::keyPressEvent(QKeyEvent *event)
{
    // Decode key event
    uint32_t modifiers = 0;
    uint32_t key = 0;

    if (event->modifiers() & Qt::ControlModifier)
        modifiers |= MODIFIER_CTRL;
    if (event->modifiers() & Qt::AltModifier)
        modifiers |= MODIFIER_ALT;
    if (event->modifiers() & Qt::MetaModifier)
        modifiers |= MODIFIER_SUPER;
    if (event->modifiers() & Qt::ShiftModifier)
        modifiers |= MODIFIER_SHIFT;

    int k = 0;
    while (keyTable[k]) {
        if (event->key() == (int)keyTable[k]) {
            key = keyTable[k + 1];
            break;
        }
        k += 2;
    }

    // Look for a matching key binding
    for (KeyBinding *keyBinding: m_shell->keyBindings()) {
        if (keyBinding->modifiers() == modifiers && keyBinding->key() == key) {
            keyBinding->send_triggered();
            break;
        }
    }

    // Call overridden method
    QWaylandCompositor::keyPressEvent(event);
}
#endif

void Compositor::setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY)
{
    if ((m_cursorSurface != surface) && surface)
        connect(surface, &QWaylandSurface::damaged, [=](const QRegion &region) {
            if (!m_cursorSurface)
                return;


            QCursor cursor(QPixmap::fromImage(m_cursorSurface->image()), m_cursorHotspotX, m_cursorHotspotY);
            static bool cursorIsSet = false;
            if (cursorIsSet) {
                QGuiApplication::changeOverrideCursor(cursor);
            } else {
                QGuiApplication::setOverrideCursor(cursor);
                cursorIsSet = true;
            }
        });

    m_cursorSurface = surface;
    m_cursorHotspotX = hotspotX;
    m_cursorHotspotY = hotspotY;
}

#include "moc_compositor.cpp"
