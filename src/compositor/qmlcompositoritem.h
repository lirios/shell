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

#ifndef QMLCOMPOSITORITEM_H
#define QMLCOMPOSITORITEM_H

#include <QtQuick/QQuickItem>

#include "compositor.h"

class QmlCompositorItemCompositor : public Compositor
{
    Q_OBJECT
public:
    explicit QmlCompositorItemCompositor(QWindow *window = 0);

    void surfaceCreated(QWaylandSurface *surface);
    void surfaceAboutToBeDestroyed(QWaylandSurface *surface);

Q_SIGNALS:
    void waylandSurfaceCreated(QWaylandSurface *surface);
    void waylandSurfaceAboutToBeDestroyed(QWaylandSurface *surface);

public Q_SLOTS:
    void frameStartedSlot();
    void sendFrameCallbacksSlot();
};

class QmlCompositorItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QmlCompositorItemCompositor *compositor READ compositor)
    Q_ENUMS(State WindowRole)
public:
    enum State {
        //! Compositor is active.
        Active,
        //! Shell unlock called on activity.
        Idle,
        //! No rendering, no frame events.
        Offscreen,
        //! Same as CompositorOffscreen, but also set DPMS
        Sleeping
    };

    enum WindowRole {
        ApplicationRole,
        LockScreenRole,
        OverlayRole,
        DialogRole,
        FullScreenRole,
        PanelRole,
        PopupRole,
        NotificationRole,
        DesktopRole,
        BackgroundRole
    };

    explicit QmlCompositorItem(QQuickItem *parent = 0);

    QmlCompositorItemCompositor *compositor() const;

    Q_INVOKABLE QPointF initialPositionForSurface(QWaylandSurface *surface);

Q_SIGNALS:
    void waylandSurfaceCreated(QWaylandSurface *surface);
    void waylandSurfaceAboutToBeDestroyed(QWaylandSurface *surface);

    void idleInhibitResetRequested();
    void idleTimerStartRequested();
    void idleTimerStopRequested();

    void idle();
    void wake();

    void ready();

    void fadeIn();
    void fadeOut();

    void locked();
    void unlocked();

public Q_SLOTS:
    void damageAll();
    void destroyClientForSurface(QWaylandSurface *surface);

protected:
    void geometryChanged(const QRectF &newGeometry,
                         const QRectF &oldGeometry) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void initCompositor();

private:
    QmlCompositorItemCompositor *m_compositor;
};

#endif // QMLCOMPOSITORITEM_H
