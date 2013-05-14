/****************************************************************************
 * This file is part of Desktop Shell.
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

#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QScreen>

#include <qpa/qplatformnativeinterface.h>

#include <VibeCore/VSettings>

#include "shellui.h"
#include "waylandintegration.h"

ShellUi::ShellUi(QScreen *screen, QObject *parent)
    : QObject(parent)
    , m_screen(screen)
    , m_backgroundWindow(0)
    , m_backgroundSurface(0)
    , m_panelWindow(0)
    , m_panelSurface(0)
    , m_launcherWindow(0)
    , m_launcherSurface(0)
{
    // Settings
    m_settings = new VSettings(QStringLiteral("org.hawaii.greenisland"));

    // Native platform interface
    m_native = QGuiApplication::platformNativeInterface();

    // Get native wl_output for the current screen
    m_output = static_cast<struct wl_output *>(
                m_native->nativeResourceForScreen("output", screen));

    // QML engine
    m_engine = new QQmlEngine(this);

    // Load QML component
    m_component = new QQmlComponent(m_engine, this);
    m_component->loadUrl(QUrl("qrc:///qml/Shell.qml"));
    if (!m_component->isReady())
        qFatal(qPrintable(m_component->errorString()));

    // Create QML component
    m_rootObject = m_component->create();
    if (!m_rootObject)
        qFatal("Couldn't create component from Shell.qml!");

    // Tell QML the screen geometry
    m_rootObject->setProperty("geometry", screen->availableGeometry());

    // Wayland integration
    WaylandIntegration *object = WaylandIntegration::instance();

    // Surface format for all shell windows
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setAlphaBufferSize(8);

    // Setup all windows
    const QObjectList objects = m_rootObject->children();
    for (int i = 0; i < objects.size(); i++) {
        QQuickWindow *window = qobject_cast<QQuickWindow *>(objects.at(i));
        if (!window)
            continue;

        if (window->objectName() == QStringLiteral("background")) {
            m_backgroundWindow = window;
            m_backgroundWindow->setFormat(format);
            m_backgroundWindow->setClearBeforeRendering(true);
            m_backgroundWindow->setScreen(screen);
            m_backgroundWindow->setFlags(m_backgroundWindow->flags() | Qt::X11BypassWindowManagerHint);
            m_backgroundWindow->create();
            m_backgroundWindow->setGeometry(screen->availableGeometry());
            m_backgroundSurface = static_cast<struct wl_surface *>(
                        m_native->nativeResourceForWindow("surface", m_backgroundWindow));
            hawaii_desktop_shell_set_background(object->shell, m_output,
                                                m_backgroundSurface);

            qDebug() << "Background for screen" << screen->name()
                     << "with geometry" << m_backgroundWindow->geometry();
        } else if (window->objectName() == QStringLiteral("panel")) {
            m_panelWindow = window;
            m_panelWindow->setFormat(format);
            m_panelWindow->setScreen(screen);
            m_panelWindow->setFlags(m_panelWindow->flags() | Qt::X11BypassWindowManagerHint);
            m_panelWindow->create();
            m_panelWindow->setGeometry(panelGeometry());
            m_panelSurface = static_cast<struct wl_surface *>(
                        m_native->nativeResourceForWindow("surface", m_panelWindow));
            hawaii_desktop_shell_set_panel(object->shell, m_output,
                                           m_panelSurface);

            qDebug() << "Panel for screen" << screen->name()
                     << "with geometry" << m_panelWindow->geometry();
        } else if (window->objectName() == QStringLiteral("launcher")) {
            m_launcherWindow = window;
            m_launcherWindow->setFormat(format);
            m_launcherWindow->setScreen(screen);
            m_launcherWindow->setFlags(m_launcherWindow->flags() | Qt::X11BypassWindowManagerHint);
            m_launcherWindow->create();
            m_launcherWindow->setGeometry(launcherGeometry());
            m_launcherSurface = static_cast<struct wl_surface *>(
                        m_native->nativeResourceForWindow("surface", m_launcherWindow));
            hawaii_desktop_shell_set_launcher(object->shell, m_output,
                                              m_launcherSurface);

            qDebug() << "Launcher for screen" << screen->name()
                     << "with geometry" << m_launcherWindow->geometry();
        }
    }

    // Check whether we created all the windows
    if (!m_backgroundWindow)
        qFatal("Invalid background window for screen \"%s\"",
               qPrintable(screen->name()));
    if (!m_backgroundSurface)
        qFatal("Invalid background surface for screen \"%s\"",
               qPrintable(screen->name()));
    if (!m_panelWindow)
        qFatal("Invalid panel window for screen \"%s\"",
               qPrintable(screen->name()));
    if (!m_panelSurface)
        qFatal("Invalid panel surface for screen \"%s\"",
               qPrintable(screen->name()));
    if (!m_launcherWindow)
        qFatal("Invalid launcher window for screen \"%s\"",
               qPrintable(screen->name()));
    if (!m_launcherSurface)
        qFatal("Invalid launcher surface for screen \"%s\"",
               qPrintable(screen->name()));

    // Set screen size and detect geometry changes
    updateScreenGeometry(screen->geometry());
    connect(screen, SIGNAL(geometryChanged(QRect)),
            this, SLOT(updateScreenGeometry(QRect)));
}

ShellUi::~ShellUi()
{
    delete m_component;
    delete m_engine;
    delete m_settings;
}

int ShellUi::panelSize() const
{
    return m_panelWindow->property("size").toInt();
}

QRect ShellUi::panelGeometry() const
{
    QRect screenGeometry = m_panelWindow->screen()->availableGeometry();
    return QRect(QPoint(0, 0), QSize(screenGeometry.width(), panelSize()));
}

int ShellUi::launcherSize() const
{
    return m_launcherWindow->property("size").toInt();
}

QRect ShellUi::launcherGeometry() const
{
    QString alignment = m_settings->value("launcher/alignment").toString();
    QRect screenGeometry = m_launcherWindow->screen()->availableGeometry();
    QPoint pt(0, 0);
    QSize size;

    if (alignment == QStringLiteral("left")) {
        size = QSize(launcherSize(), screenGeometry.height());
    } else if (alignment == QStringLiteral("right")) {
        pt.setX(screenGeometry.width() - launcherSize());
        size = QSize(launcherSize(), screenGeometry.height());
    } else if (alignment == QStringLiteral("bottom")) {
        pt.setY(screenGeometry.height() - launcherSize());
        size = QSize(screenGeometry.width(), launcherSize());
    }

    return QRect(pt, size);
}

void ShellUi::updateScreenGeometry(const QRect &geometry)
{
    // Background window has the same geometry as the screen
    m_backgroundWindow->setGeometry(geometry);

    // Update geometry property
    m_rootObject->setProperty("geometry", geometry);

    // Calculate available geometry
    QRect availableGeometry = geometry;
    QString alignment = m_settings->value("launcher/alignment").toString();

    availableGeometry.setY(panelSize());

    if (alignment == QStringLiteral("left")) {
        availableGeometry.setWidth(availableGeometry.width() - launcherSize());
        availableGeometry.setX(availableGeometry.x() + launcherSize());
    } else if (alignment == QStringLiteral("right")) {
        availableGeometry.setWidth(availableGeometry.width() - launcherSize());
    } else if (alignment == QStringLiteral("bottom")) {
        availableGeometry.setHeight(availableGeometry.height() - launcherSize());
    }

    // Update available geometry property
    m_rootObject->setProperty("availableGeometry", availableGeometry);

    // Send available geometry to the compositor
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_set_available_geometry(
                object->shell, m_output,
                availableGeometry.x(), availableGeometry.y(),
                availableGeometry.width(), availableGeometry.height());
}

void ShellUi::sendPanelGeometry()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_set_panel_geometry(object->shell, m_output,
                                            m_panelSurface,
                                            panelGeometry().x(),
                                            panelGeometry().y(),
                                            panelGeometry().width(),
                                            panelGeometry().height());
}

void ShellUi::sendLauncherGeometry()
{
    WaylandIntegration *object = WaylandIntegration::instance();
    hawaii_desktop_shell_set_launcher_geometry(object->shell, m_output,
                                               m_launcherSurface,
                                               launcherGeometry().x(),
                                               launcherGeometry().y(),
                                               launcherGeometry().width(),
                                               launcherGeometry().height());
}

#include "moc_shellui.cpp"
