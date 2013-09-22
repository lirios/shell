/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include <linux/input.h>

#include "griddesktops.h"
#include "shellsurface.h"
#include "shell.h"
#include "animation.h"
#include "workspace.h"
#include "transform.h"

#include "wayland-hawaii-server-protocol.h"

struct Grab : public ShellGrab {
    GridDesktops *effect;
};

void GridDesktops::grab_focus(struct weston_pointer_grab *base)
{

}

void GridDesktops::grab_button(struct weston_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state_w)
{
    ShellGrab *shgrab = container_of(base, ShellGrab, grab);
    Grab *grab = static_cast<Grab *>(shgrab);
    if (state_w == WL_POINTER_BUTTON_STATE_PRESSED) {
        int x = wl_fixed_to_int(base->pointer->x);
        int y = wl_fixed_to_int(base->pointer->y);

        int numWs = grab->shell->numWorkspaces();
        int numWsCols = ceil(sqrt(numWs));
        int numWsRows = ceil((float)numWs / (float)numWsCols);

        struct weston_output *out = grab->shell->getDefaultOutput();
        int cellW = out->width / numWsCols;
        int cellH = out->height / numWsRows;

        int c = x / cellW;
        int r = y / cellH;
        grab->effect->m_setWs = r * numWsCols + c;
        grab->effect->run(grab->effect->m_seat);
    }
}

const struct weston_pointer_grab_interface GridDesktops::grab_interface = {
    GridDesktops::grab_focus,
    [](struct weston_pointer_grab *grab, uint32_t time) {},
    GridDesktops::grab_button,
};

GridDesktops::GridDesktops(Shell *shell)
           : Effect(shell)
           , m_scaled(false)
           , m_grab(new Grab)
{
    m_grab->effect = this;
    m_binding = shell->bindKey(KEY_G, MODIFIER_SUPER, &GridDesktops::run, this);
}

GridDesktops::~GridDesktops()
{
    delete m_binding;
}

void GridDesktops::run(struct weston_seat *seat, uint32_t time, uint32_t key)
{
    run(seat);
}

void GridDesktops::run(struct weston_seat *ws)
{
    if (shell()->isInFullscreen()) {
        return;
    }

    int numWs = shell()->numWorkspaces();
    int numWsCols = ceil(sqrt(numWs));
    int numWsRows = ceil((float)numWs / (float)numWsCols);

    if (m_scaled) {
        shell()->showPanels();
        shell()->showNotifications();
        shell()->showOverlays();
        shell()->resetWorkspaces();
        shell()->endGrab(m_grab);
        shell()->selectWorkspace(m_setWs);
        for (int i = 0; i < numWs; ++i) {
            Workspace *w = shell()->workspace(i);

            Transform tr;
            w->setTransform(tr);
        }
    } else {
        shell()->showAllWorkspaces();
        shell()->hidePanels();
        shell()->hideNotifications();
        shell()->hideOverlays();
        shell()->startGrab(m_grab, &grab_interface, ws, WL_HAWAII_SHELL_CURSOR_ARROW);
        m_setWs = shell()->currentWorkspace()->number();
        for (int i = 0; i < numWs; ++i) {
            Workspace *w = shell()->workspace(i);

            int cws = i % numWsCols;
            int rws = i / numWsCols;

            float rx = 1.f / (float)numWsCols;
            float ry = 1.f / (float)numWsRows;
            if (rx > ry) {
                rx = ry;
            } else {
                ry = rx;
            }

            int x = cws * w->output()->width / numWsCols;
            int y = rws * w->output()->height / numWsRows;

            Transform tr;
            tr.scale(rx, rx, 1);
            tr.translate(x, y, 0);
            w->setTransform(tr);
        }
    }
    m_scaled = !m_scaled;
}
