/****************************************************************************
**
** Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>.
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Hawaii Shell.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
 * This is derived from https://codereview.qt-project.org/#change,78005
 * but it didn't have any license text so it was taken from QWaylandCompositor.
 */

#include <QtQuick/QQuickWindow>
#include <QtCompositor/QWaylandSurface>
#include <QtCompositor/QWaylandSurfaceItem>

#include "qmlcompositoritem.h"

QmlCompositorItemCompositor::QmlCompositorItemCompositor(QWindow *window)
    : Compositor(window)
{
    if (QQuickWindow *w = qobject_cast<QQuickWindow *>(window)) {
        connect(w, SIGNAL(beforeSynchronizing()),
                this, SLOT(frameStartedSlot()),
                Qt::DirectConnection);
        connect(w, SIGNAL(afterRendering()),
                this, SLOT(sendFrameCallbacksSlot()));
    }
}

void QmlCompositorItemCompositor::surfaceCreated(QWaylandSurface *surface)
{
    Compositor::surfaceCreated(surface);
    Q_EMIT waylandSurfaceCreated(surface);
}

void QmlCompositorItemCompositor::surfaceAboutToBeDestroyed(QWaylandSurface *surface)
{
    Q_EMIT waylandSurfaceAboutToBeDestroyed(surface);
}

void QmlCompositorItemCompositor::frameStartedSlot()
{
    frameStarted();
}

void QmlCompositorItemCompositor::sendFrameCallbacksSlot()
{
    sendFrameCallbacks(surfaces());
}

QmlCompositorItem::QmlCompositorItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_compositor(0)
{
    connect(this, SIGNAL(parentChanged(QQuickItem*)),
            this, SLOT(initCompositor()));
}

QmlCompositorItemCompositor *QmlCompositorItem::compositor() const
{
    return m_compositor;
}

QPointF QmlCompositorItem::initialPositionForSurface(QWaylandSurface *surface)
{
    if (!m_compositor)
        return QPointF(0, 0);

    return m_compositor->calculateInitialPosition(surface);
}

void QmlCompositorItem::damageAll()
{
    if (!m_compositor)
        return;

    for (QWaylandSurface *surface: m_compositor->surfaces()) {
        if (surface->surfaceItem())
            surface->surfaceItem()->setDamagedFlag(true);
    }
}

void QmlCompositorItem::destroyClientForSurface(QWaylandSurface *surface)
{
    if (!m_compositor)
        return;

    m_compositor->destroyClientForSurface(surface);
}

void QmlCompositorItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    m_compositor->setOutputGeometry(QRect(QPoint(0, 0), newGeometry.size().toSize()));
}

void QmlCompositorItem::initCompositor()
{
    if (!m_compositor && window()) {
        // Create compositor
        m_compositor = new QmlCompositorItemCompositor(window());
        connect(m_compositor, SIGNAL(waylandSurfaceCreated(QWaylandSurface*)),
                this, SIGNAL(waylandSurfaceCreated(QWaylandSurface*)));
        connect(m_compositor, SIGNAL(waylandSurfaceAboutToBeDestroyed(QWaylandSurface*)),
                this, SIGNAL(waylandSurfaceAboutToBeDestroyed(QWaylandSurface*)));
        connect(m_compositor, SIGNAL(idleInhibitResetRequested()),
                this, SIGNAL(idleInhibitResetRequested()));
        connect(m_compositor, SIGNAL(idleTimerStartRequested()),
                this, SIGNAL(idleTimerStartRequested()));
        connect(m_compositor, SIGNAL(idleTimerStopRequested()),
                this, SIGNAL(idleTimerStopRequested()));
        connect(m_compositor, SIGNAL(idle()),
                this, SIGNAL(idle()));
        connect(m_compositor, SIGNAL(wake()),
                this, SIGNAL(wake()));
        connect(m_compositor, SIGNAL(ready()),
                this, SIGNAL(ready()));
        connect(m_compositor, SIGNAL(fadeIn()),
                this, SIGNAL(fadeIn()));
        connect(m_compositor, SIGNAL(fadeOut()),
                this, SIGNAL(fadeOut()));
        connect(m_compositor, SIGNAL(locked()),
                this, SIGNAL(locked()));
        connect(m_compositor, SIGNAL(unlocked()),
                this, SIGNAL(unlocked()));

        // Set initial geometry
        m_compositor->setOutputGeometry(QRect(0, 0, width(), height()));
    }
}

#include "moc_qmlcompositoritem.cpp"
