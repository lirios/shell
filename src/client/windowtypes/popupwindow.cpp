/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <qpa/qplatformnativeinterface.h>

#include "popupwindow.h"
#include "popupwindow_p.h"
#include "desktopshell.h"
#include "desktopshell_p.h"

/*
 * PopupWindowPrivate
 */

PopupWindowPrivate::PopupWindowPrivate(PopupWindow *parent)
    : QtWayland::wl_hawaii_shell_surface()
    , q_ptr(parent)
{
}

void PopupWindowPrivate::hawaii_shell_surface_popup_done()
{
    Q_Q(PopupWindow);
    QMetaObject::invokeMethod(q, "hide");
}

/*
 * PopupWindow
 */

PopupWindow::PopupWindow(QWindow *parent)
    : QQuickWindow(parent)
    , d_ptr(new PopupWindowPrivate(this))
{
    // Transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(Qt::BypassWindowManagerHint);

    // Create platform window and inform the compositor about us
    create();
    setWindowType();
}

PopupWindow::~PopupWindow()
{
    Q_D(PopupWindow);
    if (d->isInitialized())
        wl_hawaii_shell_surface_destroy(d->object());
    delete d_ptr;
}

void PopupWindow::showEvent(QShowEvent *event)
{
    QQuickWindow::showEvent(event);

    if (isVisible())
        setWindowType();
}

void PopupWindow::hideEvent(QHideEvent *event)
{
    QQuickWindow::hideEvent(event);

    Q_D(PopupWindow);
    if (d->isInitialized() && !isVisible())
        wl_hawaii_shell_surface_destroy(d->object());
}

void PopupWindow::setWindowType()
{
    if (!isVisible())
        return;

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    wl_surface *surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    DesktopShellImpl *shell = DesktopShell::instance()->d_ptr->shell;
    wl_hawaii_shell_surface *shsurf = shell->set_popup(output, surface, x(), y());

    Q_D(PopupWindow);
    d->init(shsurf);
}

#include "moc_popupwindow.cpp"
