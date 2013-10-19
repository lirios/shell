/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLSCREEN_H
#define SHELLSCREEN_H

#include <QtCore/QObject>

#include <wayland-client.h>

#include "backgroundwindow.h"
#include "panelwindow.h"
#include "launcherwindow.h"
#include "overlaywindow.h"

class QScreen;

class ShellScreen : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)
public:
    explicit ShellScreen(QScreen *screen, QObject *parent = 0);

    QScreen *screen() const;
    wl_output *output() const;

    QRect availableGeometry() const;

    BackgroundWindow *backgroundWindow() const;
    PanelWindow *panelWindow() const;
    LauncherWindow *launcherWindow() const;
    OverlayWindow *overlayWindow() const;

Q_SIGNALS:
    void availableGeometryChanged(const QRect &rect);

public Q_SLOTS:
    void updateScreenGeometry(const QRect &rect);

private:
    QScreen *m_screen;
    wl_output *m_output;
    QRect m_availableGeometry;
    BackgroundWindow *m_backgroundWindow;
    PanelWindow *m_panelWindow;
    LauncherWindow *m_launcherWindow;
    OverlayWindow *m_overlayWindow;
};

#endif // SHELLSCREEN_H
