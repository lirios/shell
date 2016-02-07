/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtQuick/QQuickWindow>

#include "popupbehavior.h"

PopupBehavior::PopupBehavior(QQuickItem *parent)
    : QQuickItem(parent)
    , m_ready(false)
{
    // Update event filter every time this item is enabled/disabled
    // or shown/hidden
    connect(this, &PopupBehavior::enabledChanged,
            this, &PopupBehavior::update);
    connect(this, &PopupBehavior::visibleChanged,
            this, &PopupBehavior::update);

    // Do the same thing when the window is changed
    connect(this, &PopupBehavior::windowChanged, this, [=](QQuickWindow *) {
        updateEventFilter(true);
    });
}

bool PopupBehavior::eventFilter(QObject *object, QEvent *event)
{
    if (object == this)
        return false;

    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonDblClick:
        return processEvent(event);
    default:
        break;
    }

    return false;
}

void PopupBehavior::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged && data.item)
        update();
    QQuickItem::itemChange(change, data);
}

void PopupBehavior::updateEventFilter(bool enable)
{
    if (!enable && m_filterHost) {
        // Remove event filter from previous filter host
        m_filterHost->removeEventFilter(this);
        m_filterHost.clear();
    } else if (enable) {
        // No point in installing a filter if it was already installed
        // or we don't have a window yet
        QQuickWindow *currentWindow = window();
        if (!currentWindow || m_filterHost == currentWindow)
            return;

        // Remove filter from previous host
        if (m_filterHost)
            m_filterHost->removeEventFilter(this);

        // Install the filter onto this window
        currentWindow->installEventFilter(this);
        m_filterHost = currentWindow;
    }
}

bool PopupBehavior::processEvent(QEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    // Determine whether the click is inside our parent (the popup menu)
    QPointF scenePos = window()->contentItem()->mapToItem(this, mouseEvent->windowPos());
    if (contains(scenePos)) {
        // Emit a signal when clicking inside, this signal will be handled by the
        // popup menu which for example can close the menu; we return false here
        // so that the menu can handle the click itself (i.e. trigger a menu item action)
        Q_EMIT clickedInside();
        return false;
    } else {
        // If the user clicked outside we emit dismissed() and pass control to
        // the actual popup menu which will handle this (i.e. by closing the menu),
        // and we also return true to eat the event so that other objects won't get
        // the click
        Q_EMIT dismissed();
        return true;
    }

    return false;
}

void PopupBehavior::componentComplete()
{
    QQuickItem::componentComplete();

    // When this is called the component is completely loaded, we
    // can now filter events
    m_ready = true;
    update();
}

void PopupBehavior::update()
{
    if (!m_ready)
        return;

    updateEventFilter(isEnabled() && isVisible());
}

#include "moc_popupbehavior.cpp"
