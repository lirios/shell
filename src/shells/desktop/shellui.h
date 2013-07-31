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

#ifndef SHELLUI_H
#define SHELLUI_H

#include <QtCore/QObject>

#include <wayland-client.h>

class QScreen;
class QQuickWindow;

class ShellUi : public QObject
{
    Q_OBJECT
public:
    explicit ShellUi(QScreen *screen, QObject *parent = 0);
    ~ShellUi();

    wl_output *output() const;

    QQuickWindow *backgroundWindow() const;
    QQuickWindow *panelWindow() const;
    QQuickWindow *launcherWindow() const;

public Q_SLOTS:
    void updateScreenGeometry(const QRect &screenGeometry);

private:
    QScreen *m_screen;
    wl_output *m_output;
    QQuickWindow *m_backgroundWindow;
    QQuickWindow *m_panelWindow;
    QQuickWindow *m_launcherWindow;
};

#endif // SHELLUI_H
