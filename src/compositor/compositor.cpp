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
#include <QQmlContext>
#include <QQuickItem>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>
#include <QtCompositor/QWaylandInputDevice>

#include "compositor.h"
#include "cmakedirs.h"

Compositor::Compositor(const QRect &geometry)
    : VCompositor(this)
    , m_currentSurface(0)
    , m_shellProcess(0)
{
    // Set title
    setTitle(QStringLiteral("Hawaii Shell"));

    // Set initial geometry
    setGeometry(geometry);

    // Desktop shell protocol
    m_shell = new Shell();
    m_notifications = new Notifications();

    // Allow QML to access this compositor
    rootContext()->setContextProperty("compositor", this);

    // Load the QML code
    setSource(QUrl("qrc:///qml/Compositor.qml"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::black);
    winId();

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
}

Compositor::~Compositor()
{
    qDeleteAll(m_keyBindings);
    qDeleteAll(m_clientWindows);
    qDeleteAll(m_workspaces);
    delete m_notifications;
    delete m_shell;
}

void Compositor::runShell()
{
    // Run the shell client process
    m_shellProcess = new QProcess(this);
    connect(m_shellProcess, SIGNAL(started()),
            this, SLOT(shellStarted()));
    connect(m_shellProcess, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(shellFailed(QProcess::ProcessError)));
    connect(m_shellProcess, SIGNAL(readyReadStandardOutput()),
            this, SLOT(shellReadyReadStandardOutput()));
    connect(m_shellProcess, SIGNAL(readyReadStandardError()),
            this, SLOT(shellReadyReadStandardError()));
    connect(m_shellProcess, SIGNAL(aboutToClose()),
            this, SLOT(shellAboutToClose()));
    m_shellProcess->start(QLatin1String(INSTALL_LIBEXECDIR "/starthawaii"),
                          QStringList(), QIODevice::ReadOnly);
}

void Compositor::closeShell()
{
    m_shellProcess->close();
    delete m_shellProcess;
}

void Compositor::surfaceCreated(QWaylandSurface *surface)
{
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

void Compositor::setCurrentSurface(QWaylandSurface *surface)
{
    if (surface == m_currentSurface)
        return;
    m_currentSurface = surface;
    emit currentSurfaceChanged();
}

void Compositor::shellStarted()
{
    if (m_shellProcess)
        qDebug() << "Shell is ready!";
}

void Compositor::shellFailed(QProcess::ProcessError error)
{
    switch (error) {
        case QProcess::FailedToStart:
            qWarning("The shell process failed to start.\n"
                     "Either the invoked program is missing, or you may have insufficient permissions to run it.");
            break;
        case QProcess::Crashed:
            qWarning("The shell process crashed some time after starting successfully.");
            break;
        case QProcess::Timedout:
            qWarning("The shell process timedout.\n");
            break;
        case QProcess::WriteError:
            qWarning("An error occurred when attempting to write to the shell process.");
            break;
        case QProcess::ReadError:
            qWarning("An error occurred when attempting to read from the shell process.");
            break;
        case QProcess::UnknownError:
            qWarning("Unknown error starting the shell process!");
            break;
    }

    // Don't need it anymore because it failed
    m_shellProcess->close();
    delete m_shellProcess;
    m_shellProcess = 0;
}

void Compositor::shellReadyReadStandardOutput()
{
    if (m_shellProcess)
        printf("%s", m_shellProcess->readAllStandardOutput().constData());
}

void Compositor::shellReadyReadStandardError()
{
    if (m_shellProcess)
        fprintf(stderr, "%s", m_shellProcess->readAllStandardError().constData());
}

void Compositor::shellAboutToClose()
{
    qDebug() << "Shell is about to close...";
}

void Compositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    qDebug() << "Surface" << surface->title() << "mapped";

    // A surface without a shell surface is not a client side window
    if (!surface->hasShellSurface())
        return;

    QWaylandSurfaceItem *item = surface->surfaceItem();

    // Create a WaylandSurfaceItem from the surface
    if (!item) {
        item = new QWaylandSurfaceItem(surface, rootObject());
        item->setClientRenderingEnabled(true);
        item->setTouchEventsEnabled(true);
    }

    // Announce a window was added
    emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
}

void Compositor::surfaceUnmapped()
{
    // Set to 0 the current surface if it was unmapped
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == m_currentSurface)
        m_currentSurface = 0;

    qDebug() << "Surface" << surface->title() << "unmapped";

    // Announce this window was destroyed
    QQuickItem *item = surface->surfaceItem();
    if (item)
        emit windowDestroyed(QVariant::fromValue(item));
}

void Compositor::surfaceDestroyed(QObject *object)
{
    // Set to 0 the current surface if it was destroyed
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
    if (surface == m_currentSurface)
        m_currentSurface = 0;

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

void Compositor::resizeEvent(QResizeEvent *event)
{
    // Scale compositor output to window's size
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

#include "moc_compositor.cpp"
