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

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    struct ::wl_hawaii_popup_surface *popupSurface =
            RegistryListener::instance()->shellSurface()->set_popup(output, surface, x(), y());
    m_popupSurface->init(popupSurface);
}

void PopupQuickWindow::dismiss()
{
    if (m_popupSurface->isInitialized())
        m_popupSurface->dismiss();
}

#include "moc_popupquickwindow_p.cpp"
