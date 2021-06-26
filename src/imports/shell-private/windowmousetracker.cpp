// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QPointer>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickWindow>
#include <QScreen>

#include "windowmousetracker.h"

class WindowMouseTrackerPrivate
{
    Q_DECLARE_PUBLIC(WindowMouseTracker)
public:
    WindowMouseTrackerPrivate(WindowMouseTracker *self)
        : containsMouse(false)
        , windowSystemCursorEnabled(false)
        , q_ptr(self)
    {
        QImage cursorImage(64, 64, QImage::Format_ARGB32);
        cursorImage.fill(Qt::transparent);
        cursorPixmap = QPixmap::fromImage(cursorImage);
    }

    void reparentOverlay()
    {
        Q_Q(WindowMouseTracker);

        // Check if we've got a hold of the window where the tracker is in
        if (window.isNull())
            return;

        // QtQuick Controls 2 ApplicationWindow creates an overlay item
        // on top of your scene and this prevents WaylandMouseTracker from
        // work. Reparent the overlay to WindowMouseTracker as soon as it
        // is on a window and raise the Z order so we can track mouse events
        // and make the overlay work at the same time.
        for (QObject *object : qAsConst(window->contentItem()->children())) {
            if (object->inherits("QQuickOverlay")) {
                auto overlay = qobject_cast<QQuickItem *>(object);
                if (overlay) {
                    q->setZ(1000000);
                    overlay->setParentItem(q);
                    return;
                }
            }
        }
    }

    void handleMouseMove(const QPointF &mousePos)
    {
        Q_Q(WindowMouseTracker);

        bool xChanged = mousePos.x() != this->mousePos.x();
        bool yChanged = mousePos.y() != this->mousePos.y();

        if (xChanged || yChanged) {
            this->mousePos = mousePos;

            if (xChanged)
                Q_EMIT q->mouseXChanged();
            if (yChanged)
                Q_EMIT q->mouseYChanged();
        }
    }

    void setContainsMouse(bool hovered)
    {
        Q_Q(WindowMouseTracker);

        if (containsMouse == hovered)
            return;
        containsMouse = hovered;
        Q_EMIT q->containsMouseChanged();
    }

    void setWindowSystemCursorEnabled(bool enable)
    {
        // Check if we've got a hold of the window where the tracker is in
        if (window.isNull())
            return;

        // QtQuick Controls 2 ApplicationWindow creates an overlay item
        // on top of your scene, let's find this item and set/unset the
        // cursor of it in order to disable/enable window system cursor
        for (QObject *object : qAsConst(window->contentItem()->children())) {
            if (object->inherits("QQuickOverlay")) {
                auto overlay = qobject_cast<QQuickItem *>(object);
                if (overlay) {
                    if (enable)
                        overlay->unsetCursor();
                    else
                        overlay->setCursor(QCursor(cursorPixmap));
                    return;
                }
            }
        }
    }

    QPointer<QQuickWindow> window;
    bool containsMouse;
    bool windowSystemCursorEnabled;
    QPointF mousePos;
    QPixmap cursorPixmap;

private:
    WindowMouseTracker *q_ptr;
};

WindowMouseTracker::WindowMouseTracker(QQuickItem *parent)
    : QQuickItem(parent)
    , d_ptr(new WindowMouseTrackerPrivate(this))
{
    Q_D(WindowMouseTracker);

    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::NoButton);

    // QtQuick Controls 2 ApplicationWindow has an overlay on top of your
    // scene so we cannot use regular WaylandMouseTracker to track
    // mouse cursor position, but we can track the local position inside
    // the window which is the same as a WaylandMouseTracker anchored
    // to fill the whole window contentItem
    connect(this, &QQuickItem::windowChanged, this, [this, d](QQuickWindow *window) {
        // Remove event filter previously installed, if any
        if (!d->window.isNull()) {
            d->window->removeEventFilter(this);
            d->window.clear();
        }

        // Install this event filter when the item is on the window
        if (window) {
            window->installEventFilter(this);
            d->window = window;
            d->reparentOverlay();
            d->setWindowSystemCursorEnabled(d->windowSystemCursorEnabled);

            // Center mouse on primary screen
            if (window->screen() == QGuiApplication::primaryScreen()) {
                const QPoint pos = window->screen()->geometry().center();
                QCursor::setPos(window->screen(), pos);
                d->handleMouseMove(pos);
                d->setContainsMouse(true);
            }
        }
    });
}

WindowMouseTracker::~WindowMouseTracker()
{
    delete d_ptr;
}

qreal WindowMouseTracker::mouseX() const
{
    Q_D(const WindowMouseTracker);
    return d->mousePos.x();
}

qreal WindowMouseTracker::mouseY() const
{
    Q_D(const WindowMouseTracker);
    return d->mousePos.y();
}

bool WindowMouseTracker::containsMouse() const
{
    Q_D(const WindowMouseTracker);
    return d->containsMouse;
}

bool WindowMouseTracker::windowSystemCursorEnabled() const
{
    Q_D(const WindowMouseTracker);
    return d->windowSystemCursorEnabled;
}

void WindowMouseTracker::setWindowSystemCursorEnabled(bool enable)
{
    Q_D(WindowMouseTracker);

    if (d->windowSystemCursorEnabled == enable)
        return;

    d->windowSystemCursorEnabled = enable;
    d->setWindowSystemCursorEnabled(enable);
    Q_EMIT windowSystemCursorEnabledChanged();
}

bool WindowMouseTracker::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(WindowMouseTracker);

    // Only filter the window we are attached to
    if (watched != d->window.data())
        return QQuickItem::eventFilter(watched, event);

    // Allow only mouse events
    if (event->type() == QEvent::MouseMove)
        d->handleMouseMove(static_cast<QMouseEvent *>(event)->localPos());

    return false;
}

void WindowMouseTracker::hoverEnterEvent(QHoverEvent *event)
{
    Q_UNUSED(event);

    Q_D(WindowMouseTracker);
    d->setContainsMouse(true);
}

void WindowMouseTracker::hoverLeaveEvent(QHoverEvent *event)
{
    Q_UNUSED(event);

    Q_D(WindowMouseTracker);
    d->setContainsMouse(false);
}
