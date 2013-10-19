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

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtQuick/QQuickItem>

#include <qpa/qplatformnativeinterface.h>

#include "lockscreenwindow.h"
#include "hawaiishell.h"
#include "hawaiishell_p.h"
#include "shellscreen.h"

LockScreenWindow::LockScreenWindow()
    : QQuickView(HawaiiShell::instance()->engine(), new QWindow(QGuiApplication::primaryScreen()))
    , m_surface(0)
{
    // Set custom window type
    setFlags(flags() | Qt::BypassWindowManagerHint);

    // Load QML component
    setSource(QUrl("qrc:/qml/LockScreen.qml"));

    // Resizing the view resizes the root object
    setResizeMode(QQuickView::SizeRootObjectToView);
    setGeometry(screen()->geometry());

    // React to screen size changes and alignment changes
    connect(screen(), SIGNAL(geometryChanged(QRect)),
            this, SLOT(geometryChanged(QRect)));

    // Debugging message
    qDebug() << "-> Created LockScreen with geometry"
             << geometry();
}

wl_surface *LockScreenWindow::surface() const
{
    return m_surface;
}

void LockScreenWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    m_surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    HawaiiShellImpl *shell = HawaiiShell::instance()->d_ptr->shell;
    shell->set_lock_surface(m_surface);
}

void LockScreenWindow::geometryChanged(const QRect &rect)
{
    setGeometry(rect);
}

#include "moc_lockscreenwindow.cpp"
