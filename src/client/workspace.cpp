/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "workspace.h"
#include "workspace_p.h"
#include "shellmanager.h"

/*
 * WorkspacePrivate
 */

WorkspacePrivate::WorkspacePrivate()
    : QtWayland::wl_hawaii_workspace()
    , q_ptr(0)
    , active(false)
{
}

WorkspacePrivate::~WorkspacePrivate()
{
    wl_hawaii_workspace_destroy(object());
}

void WorkspacePrivate::hawaii_workspace_activated()
{
    Q_Q(Workspace);

    active = true;
    Q_EMIT q->activeChanged(active);
}

void WorkspacePrivate::hawaii_workspace_deactivated()
{
    Q_Q(Workspace);

    active = false;
    Q_EMIT q->activeChanged(active);
}

/*
 * Workspace
 */

Workspace::Workspace(bool active, QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkspacePrivate())
{
    Q_D(Workspace);
    d->q_ptr = this;
    d->active = active;
}

bool Workspace::isActive() const
{
    Q_D(const Workspace);
    return d->active;
}

void Workspace::activate()
{
    ShellManager::instance()->controller()->selectWorkspace(this);
}

#include "moc_workspace.cpp"
