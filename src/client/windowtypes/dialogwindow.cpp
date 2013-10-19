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

#include "dialogwindow.h"
#include "hawaiishell.h"
#include "hawaiishell_p.h"

DialogWindow::DialogWindow(QWindow *parent)
    : QQuickWindow(parent)
{
    // Transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(Qt::BypassWindowManagerHint);

    // Create platform window and inform the compositor about us
    create();
    setWindowType();

    // Hide the dialog when it's accepted or rejected
    connect(this, &DialogWindow::accepted, [=]() {
        this->deleteLater();
    });
    connect(this, &DialogWindow::rejected, [=]() {
        this->deleteLater();
    });
}

void DialogWindow::keyReleaseEvent(QKeyEvent *event)
{
    QQuickWindow::keyReleaseEvent(event);

    if (event->key() == Qt::Key_Escape)
        Q_EMIT rejected();
}

void DialogWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    wl_surface *surface = static_cast<wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    HawaiiShellImpl *shell = HawaiiShell::instance()->d_ptr->shell;
    shell->set_dialog(output, surface);
}

#include "moc_dialogwindow.cpp"
