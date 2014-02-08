/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtGui/QGuiApplication>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQuick/QQuickItem>

#include <HawaiiShell/QuickView>

#include "popupquickwindow_p.h"
#include "popupwindow.h"
#include "registrylistener.h"

/*
 * PopupSurface
 */

PopupSurface::PopupSurface(PopupWindow *popup)
    : QtWayland::wl_hawaii_popup_surface()
    , m_popup(popup)
{
}

PopupSurface::~PopupSurface()
{
    if (isInitialized())
        wl_hawaii_popup_surface_destroy(object());
}

QWindow *PopupSurface::parentWindow() const
{
    QQuickItem *parentItem = qobject_cast<QQuickItem *>(m_popup->parent());
    if (!parentItem)
        return nullptr;
    return qobject_cast<QWindow *>(parentItem->window());
}

QPointF PopupSurface::mapToParent(const QPointF &pos)
{
    QQuickItem *parentItem = qobject_cast<QQuickItem *>(m_popup->parent());
    if (!parentItem)
        return pos;
    return parentItem->mapToScene(pos);
}

void PopupSurface::hawaii_popup_surface_popup_done()
{
    // Call close() instead of hide() because the latter calls
    // PopupQuickWindow::dismiss() that breaks the grab resulting in
    // a crash because at that point it was already broken by
    // the compositor
    QMetaObject::invokeMethod(m_popup, "close", Qt::QueuedConnection);
}

/*
 * PopupQuickWindow
 */

PopupQuickWindow::PopupQuickWindow(PopupWindow *parent)
    : QQuickWindow()
    , m_popupSurface(new PopupSurface(parent))
{
    // Transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(Qt::BypassWindowManagerHint);

    // Create platform window
    create();
}

PopupQuickWindow::~PopupQuickWindow()
{
    delete m_popupSurface;
}

void PopupQuickWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    // Popup position
    QPointF pos = m_popupSurface->mapToParent(position());

    // Calculate position according to view location
    QWindow *parentWindow = m_popupSurface->parentWindow();
    if (parentWindow) {
        Hawaii::Shell::QuickView *view = qobject_cast<Hawaii::Shell::QuickView *>(parentWindow);
        if (view) {
            switch (view->location()) {
            case Hawaii::Shell::Types::LeftEdge:
                pos += QPointF(view->width(), 0);
                break;
            case Hawaii::Shell::Types::TopEdge:
                pos += QPointF(0, view->height());
                break;
            case Hawaii::Shell::Types::RightEdge:
                pos -= QPointF(width(), 0);
                break;
            case Hawaii::Shell::Types::BottomEdge:
                pos -= QPointF(0, height());
                break;
            default:
                break;
            }
        }
    }

    struct ::wl_surface *parent = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", parentWindow));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    struct ::wl_hawaii_popup_surface *popupSurface =
            RegistryListener::instance()->shellSurface()->set_popup(parent, surface,
                                                                    pos.x(), pos.y());
    m_popupSurface->init(popupSurface);
}

#include "moc_popupquickwindow_p.cpp"
