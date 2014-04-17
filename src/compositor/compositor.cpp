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

#include <QtGui/QGuiApplication>
#include <QScreen>
#include <QtCore/QVariantMap>
#include <QQmlContext>
#include <QQuickItem>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/QWaylandInputDevice>
#include <QtCompositor/private/qwlcompositor_p.h>
#include <QtCompositor/private/qwlinputdevice_p.h>
#include <QtCompositor/private/qwlpointer_p.h>

#include "compositor.h"
#include "panelmanager.h"
#include "shell.h"
#include "shellsurface.h"
#include "clientwindow.h"
#include "workspace.h"
#include "notifications.h"
#include "screensaver.h"
#include "keymap.h"

Q_GLOBAL_STATIC(Compositor, s_globalCompositor)

Compositor::Compositor()
    : GreenIsland::Compositor()
    , m_shellReady(false)
    , m_cursorSurface(nullptr)
    , m_cursorHotspotX(0)
    , m_cursorHotspotY(0)
{
    // Set title
    setTitle(QStringLiteral("Hawaii Shell"));

    // Allow QML to access this compositor
    qmlRegisterUncreatableType<Compositor>("Hawaii.Shell", 0, 1, "Compositor",
                                           QStringLiteral("Cannot create Compositor"));

    // Load the QML code
    setSource(QUrl("qrc:///qml/Compositor.qml"));

    // Create interfaces
    m_notifications = new Notifications(waylandDisplay());
    m_screenSaver = new ScreenSaver(waylandDisplay());
    m_panelManager = new PanelManager(waylandDisplay());
    m_shellSurface = new ShellSurface(waylandDisplay());
    m_shell = new Shell(waylandDisplay());
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
    connect(this, SIGNAL(surfaceMapped(QWaylandSurface*)),
            this, SLOT(surfaceMapped(QWaylandSurface*)));
    connect(this, SIGNAL(surfaceUnmapped(QWaylandSurface*)),
            this, SLOT(surfaceUnmapped(QWaylandSurface*)));
    connect(this, SIGNAL(surfaceDestroyed(QWaylandSurface*)),
            this, SLOT(surfaceDestroyed(QWaylandSurface*)));
    connect(this, SIGNAL(sceneGraphInitialized()),
            this, SLOT(sceneGraphInitialized()), Qt::DirectConnection);
    connect(this, SIGNAL(windowAdded(QVariant)),
            rootObject(), SLOT(windowAdded(QVariant)));
    connect(this, SIGNAL(windowRemoved(QVariant)),
            rootObject(), SLOT(windowRemoved(QVariant)));
    connect(this, SIGNAL(windowDestroyed(QVariant)),
            rootObject(), SLOT(windowDestroyed(QVariant)));
}

Compositor::~Compositor()
{
    qDeleteAll(m_clientWindows);
    qDeleteAll(m_workspaces);
    delete m_screenSaver;
    delete m_notifications;
    delete m_panelManager;
    delete m_shellSurface;
    delete m_shell;
}

Compositor *Compositor::instance()
{
    return s_globalCompositor();
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
    // Create application window instance
    if (surface && surface->hasShellSurface()) {
        ClientWindow *appWindow = new ClientWindow(waylandDisplay());
        appWindow->setSurface(surface);
        m_clientWindows.append(appWindow);
    }

    // Call overridden method
    GreenIsland::Compositor::surfaceCreated(surface);
}

void Compositor::destroyWindow(QVariant window)
{
    qvariant_cast<QObject *>(window)->deleteLater();
}

void Compositor::destroyClientForWindow(QVariant window)
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurfaceItem *>(
                qvariant_cast<QObject *>(window))->surface();
    destroyClientForSurface(surface);
}

void Compositor::lockSession()
{
    m_shell->lockSession();
}

void Compositor::unlockSession()
{
    m_shell->unlockSession();
}

void Compositor::surfaceMapped(QWaylandSurface *surface)
{
    // Sanity check
    if (!surface)
        return;

    // Get the surface item
    QWaylandSurfaceItem *item = surface->surfaceItem();

    // Create a WaylandSurfaceItem for this surface
    if (!item)
        item = new QWaylandSurfaceItem(surface, rootObject());

    // Enable touch events
    item->setTouchEventsEnabled(true);

    // Announce a window was added
    Q_EMIT windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void Compositor::surfaceUnmapped(QWaylandSurface *surface)
{
    // Sanity check
    if (!surface)
        return;

    // Announce this window was removed
    QWaylandSurfaceItem *item = surface->surfaceItem();
    if (item)
        Q_EMIT windowRemoved(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void Compositor::surfaceDestroyed(QWaylandSurface *surface)
{
    // Sanity check
    if (!surface)
        return;

    if (surface->hasShellSurface()) {
        // Delete application window
        for (ClientWindow *appWindow: m_clientWindows) {
            if (appWindow->surface() == surface) {
                if (m_clientWindows.removeOne(appWindow))
                    delete appWindow;
                break;
            }
        }
    }

    // Announce this window was destroyed
    QWaylandSurfaceItem *item = surface->surfaceItem();
    if (item)
        Q_EMIT windowDestroyed(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void Compositor::sceneGraphInitialized()
{
    showGraphicsInfo();
}

void Compositor::mousePressEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    // Custom event handling only of shell windows
    QPointF local;
    QWaylandSurface *targetSurface = m_shellSurface->surfaceAt(event->localPos(), &local);
    if (targetSurface && !targetSurface->hasShellSurface()) {
        defaultInputDevice()->sendMousePressEvent(event->button(), local, event->globalPos());
        event->accept();
    } else {
        GreenIsland::Compositor::mousePressEvent(event);
    }
}

void Compositor::mouseReleaseEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    // Custom event handling only of shell windows
    QPointF local;
    QWaylandSurface *targetSurface = m_shellSurface->surfaceAt(event->localPos(), &local);
    if (targetSurface && !targetSurface->hasShellSurface()) {
        defaultInputDevice()->sendMouseReleaseEvent(event->button(), local, event->globalPos());
        event->accept();
    } else {
        GreenIsland::Compositor::mouseReleaseEvent(event);
    }
}

void Compositor::mouseMoveEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    // Custom event handling only of shell windows
    QPointF local;
    QWaylandSurface *targetSurface = m_shellSurface->surfaceAt(event->localPos(), &local);
    if (targetSurface && !targetSurface->hasShellSurface()) {
        defaultInputDevice()->sendMouseMoveEvent(targetSurface, local, event->globalPos());
        event->accept();
    } else {
        GreenIsland::Compositor::mouseMoveEvent(event);
    }
}

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
    GreenIsland::Compositor::keyPressEvent(event);
}

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

#include "moc_compositor.cpp"
