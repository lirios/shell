/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2014 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#include "hawaiiclientwindow.h"
#include "shell.h"
#include "shellsurface.h"

#include "wayland-hawaii-server-protocol.h"

HawaiiClientWIndow::HawaiiClientWIndow()
    : Interface()
    , m_resource(nullptr)
    , m_state(HAWAII_SHELL_WINDOW_STATE_INACTIVE)
{
}

HawaiiClientWIndow::~HawaiiClientWIndow()
{
    destroy();
}

void HawaiiClientWIndow::added()
{
    shsurf()->typeChangedSignal.connect(this, &HawaiiClientWIndow::surfaceTypeChanged);
    shsurf()->titleChangedSignal.connect(this, &HawaiiClientWIndow::sendTitle);
    shsurf()->activeChangedSignal.connect(this, &HawaiiClientWIndow::activeChanged);
    shsurf()->mappedSignal.connect(this, &HawaiiClientWIndow::mapped);
    shsurf()->unmappedSignal.connect(this, &HawaiiClientWIndow::destroy);
}

ShellSurface *HawaiiClientWIndow::shsurf()
{
    return static_cast<ShellSurface *>(object());
}

void HawaiiClientWIndow::mapped()
{
    if (m_resource)
        return;

    ShellSurface::Type type = shsurf()->type();
    if (type == ShellSurface::Type::TopLevel)
        create();
}

void HawaiiClientWIndow::surfaceTypeChanged()
{
    ShellSurface::Type type = shsurf()->type();
    if (type == ShellSurface::Type::TopLevel) {
        if (!m_resource)
            create();
    } else {
        destroy();
    }
}

void HawaiiClientWIndow::activeChanged()
{
    if (shsurf()->isActive())
        m_state |= HAWAII_SHELL_WINDOW_STATE_ACTIVE;
    else
        m_state &= ~HAWAII_SHELL_WINDOW_STATE_ACTIVE;
    sendState();
}

void HawaiiClientWIndow::create()
{
    m_resource = wl_resource_create(Shell::instance()->shellClient(),
                                    &hawaii_window_interface, 1, 0);
    wl_resource_set_implementation(m_resource, &s_implementation, this, 0);
    hawaii_shell_send_window_mapped(Shell::instance()->shellClientResource(),
                                       m_resource,
                                       shsurf()->title().c_str(),
                                       shsurf()->identifier().c_str(),
                                       m_state);
}

void HawaiiClientWIndow::destroy()
{
    if (m_resource) {
        hawaii_window_send_unmapped(m_resource);
        wl_resource_destroy(m_resource);
        m_resource = nullptr;
    }
}

void HawaiiClientWIndow::sendState()
{
    if (m_resource)
        hawaii_window_send_state_changed(m_resource, m_state);
}

void HawaiiClientWIndow::sendTitle()
{
    if (m_resource)
        hawaii_window_send_title_changed(m_resource, shsurf()->title().c_str());
}

void HawaiiClientWIndow::setState(wl_client *client, wl_resource *resource, int32_t state)
{
    ShellSurface *s = shsurf();

    if (m_state & HAWAII_SHELL_WINDOW_STATE_MINIMIZED && !(state & HAWAII_SHELL_WINDOW_STATE_MINIMIZED)) {
        s->setMinimized(false);
    } else if (state & HAWAII_SHELL_WINDOW_STATE_MINIMIZED && !(m_state & HAWAII_SHELL_WINDOW_STATE_MINIMIZED)) {
        s->setMinimized(true);
        if (s->isActive())
            s->deactivate();
    }

    if (state & HAWAII_SHELL_WINDOW_STATE_ACTIVE &&
            !(state & HAWAII_SHELL_WINDOW_STATE_MINIMIZED))
        s->activate();

    m_state = state;
    sendState();
}

void HawaiiClientWIndow::close(wl_client *client, wl_resource *resource)
{
    shsurf()->close();
}

const struct hawaii_window_interface HawaiiClientWIndow::s_implementation = {
    wrapInterface(&HawaiiClientWIndow::setState),
    wrapInterface(&HawaiiClientWIndow::close)
};
