/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef SHELLCONTROLLER_H
#define SHELLCONTROLLER_H

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtQml/QQmlListProperty>

#include "shellclient.h"

class QScreen;
class ShellClient;
class ShellControllerPrivate;
class Window;
class Workspace;
class KeyBinding;

class ShellController : public QObject
{
    Q_OBJECT
    Q_PRIVATE_PROPERTY(d_ptr, QQmlListProperty<Window> windows READ windows NOTIFY windowsChanged)
    Q_PRIVATE_PROPERTY(d_ptr, QQmlListProperty<Workspace> workspaces READ workspaces NOTIFY workspacesChanged)
public:
    ShellController(QObject *parent = 0);
    virtual ~ShellController();

    Q_INVOKABLE QObject *service(const QString &name);

    Q_INVOKABLE KeyBinding *addKeyBinding(quint32 key, quint32 modifiers);

Q_SIGNALS:
    void windowsChanged();
    void windowActivated(Window *window);
    void windowSwitchingStarted();
    void windowSwitchingFinished();
    void windowSwitchingNext(Window *window);

    void workspaceAdded(int num);
    void workspaceRemoved(int num);
    void workspacesChanged();
    void workspaceSwitched(Workspace *workspace);

public Q_SLOTS:
    void minimizeWindows();
    void restoreWindows();
    void toggleWindows();

    void addWorkspace();
    void addWorkspaces(int num);

    void removeWorkspace(int num);

    void selectWorkspace(Workspace *workspace);

private:
    friend class ShellClient;

    Q_DECLARE_PRIVATE(ShellController)
    ShellControllerPrivate *const d_ptr;

    Q_PRIVATE_SLOT(d_ptr, void _q_windowAdded(Window *window))
    Q_PRIVATE_SLOT(d_ptr, void _q_workspaceAdded(Workspace *workspace))
    Q_PRIVATE_SLOT(d_ptr, void _q_shellReady())

private Q_SLOTS:
    void windowUnmapped(Window *window);
};

#endif // SHELLCONTROLLER_H
