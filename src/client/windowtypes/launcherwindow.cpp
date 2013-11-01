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

#include "launcherwindow.h"
#include "hawaiishell.h"
#include "hawaiishell_p.h"
#include "shellscreen.h"

LauncherWindow::LauncherWindow(ShellScreen *screen)
    : QQuickView(HawaiiShell::instance()->engine(), new QWindow(screen->screen()))
    , m_shellScreen(screen)
    , m_surface(0)
{
    // Set transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(Qt::BypassWindowManagerHint | Qt::WindowStaysOnTopHint);

    // Set Wayland window type
    create();
    setWindowType();

    // Load QML component
    setSource(QUrl("qrc:/qml/Launcher.qml"));

    // Load settings
    m_settings = new LauncherSettings(this);

    // Resizing the view resizes the root object
    setResizeMode(QQuickView::SizeRootObjectToView);
    resetGeometry();

    // React to screen size changes, icon size and alignment changes
    connect(screen->screen(), SIGNAL(geometryChanged(QRect)),
            this, SLOT(geometryChanged(QRect)));
    connect(m_settings, SIGNAL(alignmentChanged(LauncherSettings::Alignment)),
            this, SLOT(resized()));
    connect(rootObject(), SIGNAL(sizeChanged()),
            this, SLOT(resized()));

    // Make this window accessible from QML
    rootObject()->setProperty("window", QVariant::fromValue(
                                  qobject_cast<QObject *>(this)));

    // Debugging message
    qDebug() << "-> Created Launcher with geometry"
             << geometry();
}

LauncherWindow::~LauncherWindow()
{
    delete m_settings;
}

wl_surface *LauncherWindow::surface() const
{
    return m_surface;
}

LauncherSettings *LauncherWindow::settings() const
{
    return m_settings;
}

void LauncherWindow::geometryChanged(const QRect &rect)
{
    qDebug() << "Resizing Launcher because screen"
             << screen()->name() << "is now" << rect;
    resetGeometry();
}

void LauncherWindow::resetGeometry()
{
    int size = rootObject()->property("size").toInt();
    int l = screen()->geometry().left();
    int t = screen()->geometry().top();
    int w = screen()->geometry().width();
    int h = screen()->geometry().height();
    QRect rect;

    switch (m_settings->alignment()) {
    case LauncherSettings::LeftAlignment:
        rect = QRect(l, t, size, h);
        break;
    case LauncherSettings::RightAlignment:
        rect = QRect(l + w - size, t, size, h);
        break;
    case LauncherSettings::TopAlignment:
        rect = QRect(l, t, w, size);
        break;
    case LauncherSettings::BottomAlignment:
        rect = QRect(l, t + h - size, w, size);
        break;
    }

    setGeometry(rect);
    setSurfacePosition();
}

void LauncherWindow::resized()
{
    resetGeometry();
    m_shellScreen->updateScreenGeometry(screen()->geometry());
}

void LauncherWindow::setWindowType()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    wl_output *output = static_cast<struct wl_output *>(
                native->nativeResourceForScreen("output", screen()));

    m_surface = static_cast<struct wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    HawaiiShellImpl *shell = HawaiiShell::instance()->d_ptr->shell;
    shell->set_launcher(output, m_surface);
}

void LauncherWindow::setSurfacePosition()
{
    HawaiiShellImpl *shell = HawaiiShell::instance()->d_ptr->shell;
    shell->set_position(m_surface, geometry().x(), geometry().y());
}

#include "moc_launcherwindow.cpp"
