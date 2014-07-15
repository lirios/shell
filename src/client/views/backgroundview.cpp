/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/QDebug>
#include <QtGui/QGuiApplication>
#include <QtGui/QWindow>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QtQuick/QQuickItem>

#include <Hawaii/PluginLoader>

#include "backgroundview.h"
#include "shellmanager.h"

BackgroundView::BackgroundView(ShellUi *mantle, QScreen *screen)
    : QQuickView(new QWindow(screen))
    , m_settings(new QStaticConfiguration(this))
{
    // Transparent color
    setColor(Qt::transparent);

    // Set custom window type
    setFlags(Qt::BypassWindowManagerHint);

    // Create platform window
    create();

    // Set surface role
    setSurfaceRole();

    // Setup settings and react to background change
    m_settings->setCategory(QStringLiteral("shell"));
    connect(m_settings, &QStaticConfiguration::valueChanged,
            this, &BackgroundView::settingChanged);

    // Resize view to actual size and thus resize the root object
    setResizeMode(QQuickView::SizeRootObjectToView);

    // Set geometry and react to screen geometry changes
    setGeometry(screen->geometry());
    connect(screen, &QScreen::geometryChanged,
            this, static_cast<void (QWindow::*)(const QRect &)>(&QWindow::setGeometry));
    connect(this, &BackgroundView::screenChanged, [=](QScreen *screen) {
        setGeometry(screen->geometry());
        setSurfaceRole();
    });

    // Load QML source file
    setSource(QUrl::fromLocalFile(mantle->shellPackage().filePath(
                                      "views", QStringLiteral("Background.qml"))));

    // Load plugin
    const QString defaultBackground = QStringLiteral("org.hawaii.backgrounds.wallpaper");
    loadPlugin(m_settings->value(QStringLiteral("background"),
                                 defaultBackground).toString());

    // Debugging message
    qDebug() << "-> Created BackgroundView with geometry"
             << geometry();
}

void BackgroundView::loadPlugin(const QString &background)
{
    // Load another plugin only if this is the first time or the
    // previously loaded plugin is different
    if (!m_package.isValid() || m_package.path() != background) {
        // Load background plugin package
        if (!m_package.isValid())
            m_package = Hawaii::PluginLoader::instance()->loadPackage(
                        Hawaii::PluginLoader::BackgroundPlugin);
        m_package.setPath(background);
    } else {
        // If we are here it means we're trying to load the same an
        // already loaded plugin, so bail out
        return;
    }

    // If the package is invalid try with the default plugin
    if (!m_package.isValid())
        m_package.setPath(QStringLiteral("org.hawaii.backgrounds.wallpaper"));

    // Now load the QML component
    QString mainScript = m_package.filePath("mainscript");
    rootObject()->setProperty("sourceUrl", QUrl::fromLocalFile(mainScript));
}

void BackgroundView::setSurfaceRole()
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();

    struct ::wl_output *output = static_cast<struct ::wl_output *>(
                native->nativeResourceForScreen("output", screen()));
    struct ::wl_surface *surface = static_cast<struct ::wl_surface *>(
                native->nativeResourceForWindow("surface", this));

    ShellManager::instance()->shellInterface()->set_background(output, surface);
}

void BackgroundView::settingChanged(const QString &key, const QVariant &value)
{
    if (key == QStringLiteral("background"))
        loadPlugin(value.toString());
}

#include "moc_backgroundview.cpp"
