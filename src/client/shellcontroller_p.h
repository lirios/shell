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

#ifndef SHELLCONTROLLER_P_H
#define SHELLCONTROLLER_P_H

#include "shellcontroller.h"

class ShellControllerPrivate
{
    Q_DECLARE_PUBLIC(ShellController)
public:
    ShellControllerPrivate(ShellController *parent);
    ~ShellControllerPrivate();

    ShellController *q_ptr;

    QHash<QString, QObject *> services;
    QList<Window *> windowsList;
    QList<Workspace *> workspacesList;
    QList<KeyBinding *> keyBindingsList;

    bool windowsMinimized;

    void _q_windowAdded(Window *window);
    void _q_workspaceAdded(Workspace *workspace);
    void _q_shellReady();

    QQmlListProperty<Window> windows();
    QQmlListProperty<Workspace> workspaces();

    static int windowsCount(QQmlListProperty<Window> *p);
    static Window *windowAt(QQmlListProperty<Window> *p, int index);

    static int workspacesCount(QQmlListProperty<Workspace> *p);
    static Workspace *workspaceAt(QQmlListProperty<Workspace> *p, int index);
};

#endif // SHELLCONTROLLER_P_H
