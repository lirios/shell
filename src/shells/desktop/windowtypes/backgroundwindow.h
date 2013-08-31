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

#ifndef BACKGROUNDWINDOW_H
#define BACKGROUNDWINDOW_H

#include <QtQuick/QQuickView>

#include <wayland-client.h>

class ShellUi;

class BackgroundWindow : public QQuickView
{
    Q_OBJECT
public:
    BackgroundWindow(ShellUi *ui);

    wl_surface *surface() const;

private Q_SLOTS:
    void geometryChanged(const QRect &rect);

private:
    wl_surface *m_surface;

    void setWindowType();
    void setSurfacePosition();
};

#endif // BACKGROUNDWINDOW_H
