// SPDX-FileCopyrightText: 2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "windowmask.h"
#include "shellhelperlogging_p.h"

WindowMask::WindowMask(QObject *parent)
    : QObject(parent)
{
}

QWindow *WindowMask::window() const
{
    return m_window;
}

void WindowMask::setWindow(QWindow *window)
{
    if (m_window == window)
        return;

    if (m_initialized) {
        qCWarning(lcShellHelper, "Unable to change WindowMask::window after initialization");
        return;
    }

    m_window = window;
    Q_EMIT windowChanged(m_window);
}

void WindowMask::addRect(const QRect &rect)
{
    m_region += rect;

    if (m_window)
        m_window->setMask(m_region);
}

void WindowMask::removeRect(const QRect &rect)
{
    m_region -= rect;

    if (m_window)
        m_window->setMask(m_region);
}

void WindowMask::componentComplete()
{
    if (m_initialized)
        return;

    // Find the window from the parent, if not specified
    if (!m_window) {
        for (auto *p = parent(); p != nullptr; p = p->parent()) {
            if (auto *w = qobject_cast<QWindow *>(p)) {
                setWindow(w);
                break;
            }
        }
    }

    // Initialize
    m_initialized = true;
}
