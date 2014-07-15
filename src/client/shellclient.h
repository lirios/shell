/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLCLIENT_H
#define SHELLCLIENT_H

#include <QtCore/QObject>

#include "qwayland-hawaii.h"

class QCursor;
class Window;
class Workspace;

class ShellClient : public QObject, public QtWayland::hawaii_shell
{
    Q_OBJECT
public:
    ShellClient(QObject *parent = 0);

Q_SIGNALS:
    void loaded();
    void prepareLockSurface();
    void cursorChanged(const QCursor &cursor);
    void windowAdded(Window *window);
    void windowSwitchingStarted();
    void windowSwitchingFinished();
    void switchedToWindow(Window *window);
    void workspaceAdded(Workspace *workspace);

protected:
    void shell_loaded() Q_DECL_OVERRIDE;
    void shell_configure(struct ::wl_surface *target,
                         int32_t width,
                         int32_t height);
    void shell_prepare_lock_surface() Q_DECL_OVERRIDE;
    void shell_grab_cursor(uint32_t cursor) Q_DECL_OVERRIDE;
    void shell_window_mapped(struct ::hawaii_window *id,
                             const QString &title,
                             const QString &identifier,
                             int32_t state) Q_DECL_OVERRIDE;
    void shell_window_switching_started() Q_DECL_OVERRIDE;
    void shell_window_switching_finished() Q_DECL_OVERRIDE;
    void shell_window_switched(struct ::hawaii_window *window);
    void shell_workspace_added(struct ::hawaii_workspace *workspace,
                               int32_t active) Q_DECL_OVERRIDE;
};

#endif // SHELLCLIENT_H
