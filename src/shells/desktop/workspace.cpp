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

#include "workspace.h"
#include "workspace_p.h"
#include "waylandintegration.h"
#include "utils.h"

/*
 * WorkspacePrivate
 */

WorkspacePrivate::WorkspacePrivate()
    : q_ptr(0)
    , active(false)
    , workspace(0)
{
}

void WorkspacePrivate::initialize(hawaii_workspace *workspace, bool active)
{
    this->workspace = workspace;
    this->active = active;
    hawaii_workspace_add_listener(this->workspace, &m_listener, this);
}

void WorkspacePrivate::handleActivated(hawaii_workspace *workspace)
{
    Q_Q(Workspace);
    Q_UNUSED(workspace);

    active = true;
    Q_EMIT q->activeChanged(active);
}

void WorkspacePrivate::handleDeactivated(hawaii_workspace *workspace)
{
    Q_Q(Workspace);
    Q_UNUSED(workspace);

    active = false;
    Q_EMIT q->activeChanged(active);
}

const hawaii_workspace_listener WorkspacePrivate::m_listener = {
    wrapInterface(&WorkspacePrivate::handleActivated),
    wrapInterface(&WorkspacePrivate::handleDeactivated)
};

/*
 * Workspace
 */

Workspace::Workspace(QObject *parent)
    : QObject(parent)
    , d_ptr(new WorkspacePrivate())
{
    Q_D(Workspace);
    d->q_ptr = this;
}

bool Workspace::isActive() const
{
    Q_D(const Workspace);
    return d->active;
}

void Workspace::activate()
{
    Q_D(Workspace);

    WaylandIntegration *integration = WaylandIntegration::instance();
    hawaii_desktop_shell_select_workspace(integration->shell, d->workspace);
}

#include "moc_workspace.cpp"
