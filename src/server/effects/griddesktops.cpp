/*
 * Copyright 2013  Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/input.h>

#include "griddesktops.h"
#include "shellsurface.h"
#include "shell.h"
#include "animation.h"
#include "workspace.h"
#include "transform.h"
#include "binding.h"

struct DGrab : public ShellGrab {
    void focus() override
    {
        if (moving) {
            return;
        }

        wl_fixed_t sx, sy;
        weston_view *view = weston_compositor_pick_view(pointer()->seat->compositor, pointer()->x, pointer()->y, &sx, &sy);

        if (pointer()->focus != view) {
            weston_pointer_set_focus(pointer(), view, sx, sy);
        }
    }
    void motion(uint32_t time, wl_fixed_t x, wl_fixed_t y) override
    {
        weston_pointer_move(pointer(), x, y);

        if (surface) {
            int pos_x = wl_fixed_to_int(pointer()->x + dx);
            int pos_y = wl_fixed_to_int(pointer()->y + dy);
            if (!moving) {
                int dx = pos_x - surface->x();
                int dy = pos_y - surface->y();
                if (fabsf(dx) + fabsf(dy) < 5) {
                    return;
                }
                moving = true;

                surface->workspace()->removeSurface(surface);
                Shell::instance()->putInLimbo(surface);
                weston_matrix *matrix = &surfTransform.matrix;
                weston_matrix_init(matrix);
                weston_matrix_scale(matrix, scale, scale, 1.f);

                surface->setPosition(surface->transformedX(), surface->transformedY());
                surface->addTransform(&surfTransform);
                surface->moveStartSignal(surface);
                setCursor(Cursor::Move);
            }

            surface->setPosition(pos_x, pos_y);
        }

        if (!moving) {
            wl_list *resource_list = &pointer()->focus_resource_list;
            wl_resource *resource;
            wl_resource_for_each(resource, resource_list) {
                wl_fixed_t sx, sy;
                weston_view_from_global_fixed(pointer()->focus, pointer()->x, pointer()->y, &sx, &sy);
                wl_pointer_send_motion(resource, time, sx, sy);
            }
        }
    }
    void button(uint32_t time, uint32_t button, uint32_t state) override
    {
        if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
            ShellSurface *shsurf = pointer()->focus ? Shell::getShellSurface(pointer()->focus->surface) : nullptr;
            if (shsurf) {
                dx = wl_fixed_from_double(shsurf->transformedX()) - pointer()->grab_x;
                dy = wl_fixed_from_double(shsurf->transformedY()) - pointer()->grab_y;
                surface = shsurf;
                moving = false;
                wl_list_init(&surfTransform.link);
            }
        } else {
            int x = wl_fixed_to_int(pointer()->x);
            int y = wl_fixed_to_int(pointer()->y);
            int numWs = Shell::instance()->numWorkspaces();
            int ws = 0;
            for (int i = 0; i < numWs; ++i) {
                Workspace *w = Shell::instance()->workspace(i);
                if (w->boundingBox().contains(x, y)) {
                    ws = i;
                    break;
                }
            }

            if (surface && moving) {
                surface->removeTransform(&surfTransform);
                Workspace *w = Shell::instance()->workspace(ws);
                w->addSurface(surface);

                float x = wl_fixed_to_int(pointer()->x + dx);
                float y = wl_fixed_to_int(pointer()->y + dy);
                IRect2D bbox = w->boundingBox();
                surface->setPosition((int)((x - bbox.x) / scale) , (int)((y - bbox.y) / scale));
                surface->moveEndSignal(surface);
                unsetCursor();
            } else {
                effect->m_setWs = ws;
                effect->run(effect->m_seat);
                effect->binding("Toggle")->releaseToggle();
            }
            surface = nullptr;
            moving = false;
        }
    }

    GridDesktops *effect;
    ShellSurface *surface;
    wl_fixed_t dx, dy;
    bool moving;
    weston_transform surfTransform;
    float scale;
};

GridDesktops::GridDesktops()
           : Effect()
           , m_scaled(false)
           , m_grab(new DGrab)
{
    m_grab->effect = this;
    Binding *b = new Binding();
    b->setIsToggle(true);
    b->keyTriggered.connect(this, &GridDesktops::run);
    b->hotSpotTriggered.connect(this, &GridDesktops::run);
    addBinding("Toggle", b);
}

GridDesktops::~GridDesktops()
{
}

void GridDesktops::run(struct weston_seat *seat, uint32_t time, uint32_t key)
{
    run(seat);
}

void GridDesktops::run(weston_seat *seat, uint32_t time, Binding::HotSpot hs)
{
    run(seat);
}

void GridDesktops::run(struct weston_seat *ws)
{
    Shell *shell = Shell::instance();
    if (shell->isInFullscreen()) {
        return;
    }

    int numWs = shell->numWorkspaces();
    int numWsCols = ceil(sqrt(numWs));
    int numWsRows = ceil((float)numWs / (float)numWsCols);

    if (m_scaled) {
        shell->showPanels();
        shell->resetWorkspaces();
        m_grab->end();
        shell->selectWorkspace(m_setWs);
        for (int i = 0; i < numWs; ++i) {
            Workspace *w = shell->workspace(i);

            Transform tr;
            w->setTransform(tr);
        }
    } else {
        shell->showAllWorkspaces();
        shell->hidePanels();
        m_grab->surface = nullptr;
        m_grab->start(ws, Cursor::Arrow);
        m_setWs = shell->currentWorkspace()->number();

        weston_output *out = shell->currentWorkspace()->output();
        const int margin_w = out->width / 70;
        const int margin_h = out->height / 70;

        float rx = (1.f - (1 + numWsCols) * margin_w / (float)out->width) / (float)numWsCols;
        float ry = (1.f - (1 + numWsRows) * margin_h / (float)out->width) / (float)numWsRows;
        if (rx > ry) {
            rx = ry;
        } else {
            ry = rx;
        }
        m_grab->scale = rx;

        for (int i = 0; i < numWs; ++i) {
            Workspace *w = shell->workspace(i);

            int cws = i % numWsCols;
            int rws = i / numWsCols;

            int x = cws * (out->width - margin_w * (1 + numWsCols)) / numWsCols + (1 + cws) * margin_w;
            int y = rws * (out->height - margin_h * (1 + numWsRows)) / numWsRows + (1 + rws) * margin_h;

            Transform tr;
            tr.scale(rx, rx, 1);
            tr.translate(x, y, 0);
            w->setTransform(tr);
        }
    }
    m_scaled = !m_scaled;
}


GridDesktops::Settings::Settings()
           : Effect::Settings()
           , m_effect(nullptr)
{
}

GridDesktops::Settings::~Settings()
{
    delete m_effect;
}

std::list<Option> GridDesktops::Settings::options() const
{
    auto list = Effect::Settings::options();
    list.push_back(Option::binding("toggle_binding", Binding::Type::Key | Binding::Type::HotSpot));

    return list;
}

void GridDesktops::Settings::unSet(const std::string &name)
{
    if (name == "enabled") {
        delete m_effect;
        m_effect = nullptr;
    } else if (name == "toggle_binding") {
        m_effect->binding("Toggle")->reset();
    }
}

void GridDesktops::Settings::set(const std::string &name, int v)
{
    if (name == "enabled") {
        if (v && !m_effect) {
            m_effect = new GridDesktops;
            const Option *o = option("toggle_binding");
            o->valueAsBinding().bind(m_effect->binding("Toggle"));
        } else if (!v) {
            delete m_effect;
            m_effect = nullptr;
        }
    }
}

void GridDesktops::Settings::set(const std::string &name, const Option::BindingValue &v)
{
    if (name == "toggle_binding" && m_effect) {
        v.bind(m_effect->binding("Toggle"));
    }
}

SETTINGS(griddesktops_effect, GridDesktops::Settings)
