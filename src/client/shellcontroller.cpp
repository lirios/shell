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

#include <QtGui/QCursor>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformnativeinterface.h>

#include "shellcontroller.h"
#include "shellcontroller_p.h"
#include "keybinding.h"
#include "keybinding_p.h"
#include "registration.h"
#include "servicefactory.h"
#include "shellmanager.h"
#include "window.h"
#include "workspace.h"
#include "workspace_p.h"

/*
 * ShellControllerPrivate
 */

ShellControllerPrivate::ShellControllerPrivate(ShellController *parent)
    : q_ptr(parent)
    , windowsMinimized(false)
{
}

ShellControllerPrivate::~ShellControllerPrivate()
{
    qDeleteAll(windowsList);
    qDeleteAll(workspacesList);
    qDeleteAll(keyBindingsList);
}

void ShellControllerPrivate::_q_windowAdded(Window *window)
{
    Q_Q(ShellController);

    windowsList.append(window);

    QObject::connect(window, &Window::unmapped,
                     q, &ShellController::windowUnmapped);
    QObject::connect(window, &Window::activeChanged, [=](bool active) {
        Q_EMIT q->windowActivated(window);
    });

    Q_EMIT q->windowsChanged();
}

void ShellControllerPrivate::_q_workspaceAdded(Workspace *workspace)
{
    Q_Q(ShellController);

    workspacesList.append(workspace);

    Q_EMIT q->workspaceAdded(workspacesList.indexOf(workspace));
    Q_EMIT q->workspacesChanged();

    q->connect(workspace, &Workspace::activeChanged, [=](bool active){
        Q_EMIT q->workspaceSwitched(workspace);
    });
}

void ShellControllerPrivate::_q_shellReady()
{
    ShellManager::instance()->shellInterface()->desktop_ready();
}

QQmlListProperty<Window> ShellControllerPrivate::windows()
{
    Q_Q(ShellController);
    return QQmlListProperty<Window>(q, 0, windowsCount, windowAt);
}

QQmlListProperty<Workspace> ShellControllerPrivate::workspaces()
{
    Q_Q(ShellController);
    return QQmlListProperty<Workspace>(q, 0, workspacesCount, workspaceAt);
}

int ShellControllerPrivate::windowsCount(QQmlListProperty<Window> *p)
{
    ShellController *controller = static_cast<ShellController *>(p->object);
    return controller->d_ptr->windowsList.size();
}

Window *ShellControllerPrivate::windowAt(QQmlListProperty<Window> *p, int index)
{
    ShellController *controller = static_cast<ShellController *>(p->object);
    return controller->d_ptr->windowsList.at(index);
}

int ShellControllerPrivate::workspacesCount(QQmlListProperty<Workspace> *p)
{
    ShellController *controller = static_cast<ShellController *>(p->object);
    return controller->d_ptr->workspacesList.size();
}

Workspace *ShellControllerPrivate::workspaceAt(QQmlListProperty<Workspace> *p, int index)
{
    ShellController *controller = static_cast<ShellController *>(p->object);
    return controller->d_ptr->workspacesList.at(index);
}

/*
 * ShellController
 */

ShellController::ShellController(QObject *parent)
    : QObject(parent)
    , d_ptr(new ShellControllerPrivate(this))
{
    ShellManager *manager = ShellManager::instance();
    ShellClient *client = manager->shellInterface();

    connect(client, &ShellClient::loaded,
            manager, &ShellManager::create);
    connect(client, SIGNAL(windowAdded(Window*)),
            this, SLOT(_q_windowAdded(Window*)));
    connect(client, &ShellClient::windowSwitchingStarted,
            this, &ShellController::windowSwitchingStarted);
    connect(client, &ShellClient::windowSwitchingFinished,
            this, &ShellController::windowSwitchingFinished);
    connect(client, &ShellClient::switchedToWindow,
            this, &ShellController::windowSwitchingNext);
    connect(client, SIGNAL(workspaceAdded(Workspace*)),
            this, SLOT(_q_workspaceAdded(Workspace*)));
    connect(manager, SIGNAL(ready()), this, SLOT(_q_shellReady()));
}

ShellController::~ShellController()
{
    delete d_ptr;
}

QObject *ShellController::service(const QString &name)
{
    Q_D(ShellController);

    // Get an already created service
    QObject *service = d->services.value(name);
    if (service)
        return service;

    // If we can't find it just create it
    service = ServiceFactory::createService(name, this);
    d->services[name] = service;
    return service;
}

KeyBinding *ShellController::addKeyBinding(quint32 key, quint32 modifiers)
{
    Q_D(ShellController);

    ShellClient *client = ShellManager::instance()->shellInterface();
    KeyBinding *keyBinding = new KeyBinding(key, modifiers, this);
    keyBinding->d_ptr->init(client->add_key_binding(key, modifiers));
    d->keyBindingsList.append(keyBinding);
    return keyBinding;
}

void ShellController::minimizeWindows()
{
    Q_D(ShellController);
    ShellManager::instance()->shellInterface()->minimize_windows();
    d->windowsMinimized = true;
}

void ShellController::restoreWindows()
{
    Q_D(ShellController);
    ShellManager::instance()->shellInterface()->restore_windows();
    d->windowsMinimized = false;
}

void ShellController::toggleWindows()
{
    Q_D(ShellController);
    if (d->windowsMinimized)
        restoreWindows();
    else
        minimizeWindows();
}

void ShellController::addWorkspace()
{
    ShellManager::instance()->shellInterface()->add_workspace();
}

void ShellController::addWorkspaces(int num)
{
    while (num--)
        ShellManager::instance()->shellInterface()->add_workspace();
}

void ShellController::removeWorkspace(int num)
{
    Q_D(ShellController);

    Workspace *workspace = d->workspacesList.takeAt(num);
    if (workspace) {
        Q_EMIT workspaceRemoved(num);
        delete workspace;
    }
}

void ShellController::selectWorkspace(Workspace *workspace)
{
    struct ::wl_hawaii_workspace *w = workspace->d_ptr->object();
    ShellManager::instance()->shellInterface()->select_workspace(w);
}

void ShellController::windowUnmapped(Window *window)
{
    Q_D(ShellController);

    d->windowsList.removeOne(window);
    Q_EMIT windowsChanged();
}

#include "moc_shellcontroller.cpp"
