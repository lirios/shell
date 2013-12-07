/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QDebug>
#include <QGuiApplication>
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
#include "shell.h"
#include "keybinding.h"
#include "clientwindow.h"
#include "workspace.h"
#include "grab.h"
#include "notifications.h"

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

    connect(this, SIGNAL(shellWindowAdded(QVariant)),
            rootObject(), SLOT(shellWindowAdded(QVariant)));
    connect(this, SIGNAL(windowAdded(QVariant)),
            rootObject(), SLOT(windowAdded(QVariant)));
    connect(this, SIGNAL(windowDestroyed(QVariant)),
            rootObject(), SLOT(windowDestroyed(QVariant)));
    connect(this, SIGNAL(windowResized(QVariant)),
            rootObject(), SLOT(windowResized(QVariant)));
    connect(this, SIGNAL(sceneGraphInitialized()),
            this, SLOT(sceneGraphInitialized()), Qt::DirectConnection);
    connect(this, SIGNAL(frameSwapped()),
            this, SLOT(frameSwapped()));

    // Protocols
    m_shell = new Shell(waylandDisplay());
    connect(m_shell, &Shell::ready, [=]() {
        // Shell is ready and we can start handling input events
        m_shellReady = true;

        // Fade in the desktop
        Q_EMIT ready();
    });
    m_notifications = new Notifications(waylandDisplay());
}

Compositor::~Compositor()
{
    qDeleteAll(m_keyBindings);
    qDeleteAll(m_clientWindows);
    qDeleteAll(m_workspaces);
    delete m_notifications;
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

bool Compositor::isShellWindow(QWaylandSurface *surface)
{
    // Sanity check
    if (!surface)
        return false;

    // Shell user interface surfaces don't have a shell surface just
    // like application windows but we must draw them
    return (surface->windowFlags() & QWaylandSurface::BypassWindowManager) &&
            !surface->hasShellSurface() &&
            surface->windowProperties().contains(QStringLiteral("role"));
}

void Compositor::surfaceCreated(QWaylandSurface *surface)
{
    // Create application window instance
    ClientWindow *appWindow = new ClientWindow(waylandDisplay());
    appWindow->setSurface(surface);
    m_clientWindows.append(appWindow);

    // Connect surface signals
    connect(surface, SIGNAL(destroyed(QObject *)),
            this, SLOT(surfaceDestroyed(QObject *)));
    connect(surface, SIGNAL(mapped()),
            this, SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()),
            this, SLOT(surfaceUnmapped()));
}

void Compositor::surfaceAboutToBeDestroyed(QWaylandSurface *surface)
{
    // TODO:
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

void Compositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    // Ignore surfaces which are neither application windows nor shell windows
    if (!surface->hasShellSurface() && !isShellWindow(surface))
        return;

    // Get the surface item
    QWaylandSurfaceItem *item = surface->surfaceItem();

    // Create a WaylandSurfaceItem for this surface
    if (!item)
        item = new QWaylandSurfaceItem(surface, rootObject());

    // Set surface item up
    item->setClientRenderingEnabled(true);
    item->setTouchEventsEnabled(true);

    // Setup shell windows
    if (isShellWindow(surface)) {
        // Set z-index for shell windows according to the role
        switch (surface->windowProperties().value(QStringLiteral("role")).toInt()) {
        case Shell::BackgroundWindowRole:
            item->setZ(0);
            break;
        case Shell::PanelWindowRole:
        case Shell::LauncherWindowRole:
        case Shell::SpecialWindowRole:
        case Shell::PopupWindowRole:
            item->setZ(2);
            break;
        case Shell::OverlayWindowRole:
            //item->setZ(3);
            break;
        case Shell::DialogWindowRole:
            item->setZ(4);
            break;
        default:
            break;
        }

        // Set position as asked by the shell client
        item->setPosition(surface->windowProperties().value(QStringLiteral("position")).toPointF());

        // Announce a window was added
        emit shellWindowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
    } else {
        // Set application window position
#if 0
        switch (surface->windowType()) {
        case QWaylandSurface::Toplevel:
            surface->setPos(calculateInitialPosition(surface));
            break;
        default:
            break;
        }
#endif

        // Announce a window was added
        emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
    }
}

void Compositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    // Announce this window was unmapped
    QQuickItem *item = surface->surfaceItem();
    if (item)
        emit windowDestroyed(QVariant::fromValue(item));
}

void Compositor::surfaceDestroyed(QObject *object)
{
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);

    // Announce this window was destroyed
    QQuickItem *item = surface->surfaceItem();
    if (item)
        emit windowDestroyed(QVariant::fromValue(item));
}

void Compositor::sceneGraphInitialized()
{
    showGraphicsInfo();
}

void Compositor::frameSwapped()
{
    frameFinished();
}

void Compositor::mousePressEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    QPointF local;
    QWaylandSurface *targetSurface = m_shell->surfaceAt(event->localPos(), &local);
    if (targetSurface) {
        defaultInputDevice()->sendMousePressEvent(event->button(), local, event->globalPos());
        event->accept();
    } else {
        QQuickView::mousePressEvent(event);
    }
}

void Compositor::mouseReleaseEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    QPointF local;
    QWaylandSurface *targetSurface = m_shell->surfaceAt(event->localPos(), &local);
    if (targetSurface) {
        defaultInputDevice()->sendMouseReleaseEvent(event->button(), local, event->globalPos());
        event->accept();
    } else {
        QQuickView::mouseReleaseEvent(event);
    }
}

void Compositor::mouseMoveEvent(QMouseEvent *event)
{
    // Ignore events until the shell is ready
    if (!m_shellReady) {
        event->ignore();
        return;
    }

    QPointF local;
    QWaylandSurface *targetSurface = m_shell->surfaceAt(event->localPos(), &local);
    if (targetSurface) {
        defaultInputDevice()->sendMouseMoveEvent(targetSurface, local, event->globalPos());
        event->accept();
    } else {
        QQuickView::mouseMoveEvent(event);
    }
}

void Compositor::setCursorSurface(QWaylandSurface *surface, int hotspotX, int hotspotY)
{
    if ((m_cursorSurface != surface) && surface)
        connect(surface, &QWaylandSurface::damaged, [=](const QRect &rect) {
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
