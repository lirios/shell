/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef LAUNCHERWINDOW_H
#define LAUNCHERWINDOW_H

#include <QtQuick/QQuickView>

#include <libhawaiicore/settings/launchersettings.h>

struct wl_surface;

class ShellScreen;

class LauncherWindow : public QQuickView
{
    Q_OBJECT
public:
    LauncherWindow(ShellScreen *screen);
    ~LauncherWindow();

    wl_surface *surface() const;

    LauncherSettings *settings() const;

private Q_SLOTS:
    void geometryChanged(const QRect &rect);
    void resetGeometry();
    void resized();

private:
    ShellScreen *m_shellScreen;
    wl_surface *m_surface;
    LauncherSettings *m_settings;

    void setWindowType();
    void setSurfacePosition();
};

#endif // LAUNCHERWINDOW_H
