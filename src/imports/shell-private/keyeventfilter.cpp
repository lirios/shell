// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtQuick/QQuickWindow>

#include "keyeventfilter.h"

KeyEventFilter::KeyEventFilter(QQuickItem *parent)
    : QQuickItem(parent)
{
    connect(this, &QQuickItem::windowChanged, this, [=](QQuickWindow *window) {
        // Remove event filter previously installed if any
        if (!m_window.isNull()) {
            m_window->removeEventFilter(this);
            m_window.clear();
        }

        // Install this event filter when the item is on the window
        if (window) {
            window->installEventFilter(this);
            m_window = window;
        }
    });
}

bool KeyEventFilter::eventFilter(QObject *object, QEvent *event)
{
    // Only filter the window we are attached to
    if (object != m_window.data())
        return QObject::eventFilter(object, event);

    // Discard events not related to keyboard
    if (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease)
        return false;

    // Pass this event to QML for processing, but do not eat it so it
    // will still be delivered to the currently focused application window
    event->accept();
    QCoreApplication::sendEvent(this, event);
    return event->isAccepted();
}
