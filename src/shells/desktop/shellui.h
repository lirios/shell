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

#ifndef SHELLUI_H
#define SHELLUI_H

#include <QObject>

#include <wayland-client.h>

class QPlatformNativeInterface;
class QScreen;
class QQmlEngine;
class QQmlComponent;
class QQuickWindow;

class ShellUi : public QObject
{
    Q_OBJECT
public:
    explicit ShellUi(QScreen *screen, QObject *parent = 0);
    ~ShellUi();

    // Wayland screen output
    struct wl_output *output() const {
        return m_output;
    }

    // Background

    QQuickWindow *backgroundWindow() const {
        return m_backgroundWindow;
    }

    struct wl_surface *backgroundSurface() const {
        return m_backgroundSurface;
    }

    // Panel

    int panelSize() const;

    QQuickWindow *panelWindow() const {
        return m_panelWindow;
    }

    struct wl_surface *panelSurface() const {
        return m_panelSurface;
    }

    // Launcher

    int launcherSize() const;

    QQuickWindow *launcherWindow() const {
        return m_launcherWindow;
    }

    struct wl_surface *launcherSurface() const {
        return m_launcherSurface;
    }

public Q_SLOTS:
    void updateScreenGeometry(const QRect &screenGeometry);
    void sendPanelGeometry();
    void sendLauncherGeometry();

private:
    QPlatformNativeInterface *m_native;
    QScreen *m_screen;
    struct wl_output *m_output;
    QQmlEngine *m_engine;
    QQmlComponent *m_component;
    QObject *m_rootObject;
    QQuickWindow *m_backgroundWindow;
    struct wl_surface *m_backgroundSurface;
    QQuickWindow *m_panelWindow;
    struct wl_surface *m_panelSurface;
    QQuickWindow *m_launcherWindow;
    struct wl_surface *m_launcherSurface;
};

#endif // SHELLUI_H
