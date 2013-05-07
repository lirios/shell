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

#include "scaleeffect.h"
#include "shellsurface.h"
#include "shell.h"
#include "animation.h"
#include "animationcurve.h"

#include "wayland-desktop-shell-server-protocol.h"

const float INACTIVE_ALPHA = 0.8;
const int ALPHA_ANIM_DURATION = 200;

struct Grab : public ShellGrab {
    ScaleEffect *effect;
};

struct SurfaceTransform {
    void updateAnimation(float value);
    void doneAnimation();

    ShellSurface *surface;
    struct weston_transform transform;
    Animation animation;
    Animation alphaAnim;

    float ss, ts, cs;
    int sx, tx, cx;
    int sy, ty, cy;
};

void ScaleEffect::grab_focus(struct wl_pointer_grab *base, struct wl_surface *surf, wl_fixed_t x, wl_fixed_t y)
{
    ShellGrab *shgrab = container_of(base, ShellGrab, grab);
    Grab *grab = static_cast<Grab *>(shgrab);

    Workspace *currWs = grab->shell->currentWorkspace();
    struct weston_surface *surface = container_of(surf, struct weston_surface, surface);
    for (SurfaceTransform *tr: grab->effect->m_surfaces) {
        if (tr->surface->workspace() != currWs) {
            continue;
        }

        float alpha = (tr->surface->is(surface) ? 1.0 : INACTIVE_ALPHA);
        float curr = tr->surface->alpha();
        if (alpha == curr) {
            continue;
        }

        tr->alphaAnim.setStart(curr);
        tr->alphaAnim.setTarget(alpha);
        tr->alphaAnim.run(tr->surface->output(), ALPHA_ANIM_DURATION);
    }
}

static void grab_button(struct wl_pointer_grab *base, uint32_t time, uint32_t button, uint32_t state_w)
{
    ShellGrab *shgrab = container_of(base, ShellGrab, grab);
    Grab *grab = static_cast<Grab *>(shgrab);
    if (state_w == WL_POINTER_BUTTON_STATE_PRESSED) {
        struct weston_surface *surface = (struct weston_surface *) base->pointer->current;
        ShellSurface *shsurf = grab->shell->getShellSurface(surface);
        if (shsurf) {
            grab->effect->end(shsurf);
        }
    }
}

const struct wl_pointer_grab_interface ScaleEffect::grab_interface = {
    ScaleEffect::grab_focus,
    [](struct wl_pointer_grab *grab, uint32_t time, wl_fixed_t x, wl_fixed_t y) {},
    grab_button,
};

ScaleEffect::ScaleEffect(Shell *shell)
           : Effect(shell)
           , m_scaled(false)
           , m_grab(new Grab)
{
    m_grab->effect = this;
    m_binding = shell->bindKey(KEY_E, MODIFIER_CTRL, &ScaleEffect::run, this);
}

ScaleEffect::~ScaleEffect()
{
    delete m_binding;
}

void ScaleEffect::run(struct wl_seat *seat, uint32_t time, uint32_t key)
{
    run((struct weston_seat *)seat);
}

void ScaleEffect::run(struct weston_seat *ws)
{
    int num = m_surfaces.size();
    if ((num == 0 && !m_scaled) || shell()->isInFullscreen()) {
        return;
    }

    num = 0;
    Workspace *currWs = shell()->currentWorkspace();
    for (SurfaceTransform *surf: m_surfaces) {
        if (surf->surface->workspace() == currWs) {
            ++num;
        }
    }

    if (num == 0) {
        return;
    }

    const int ANIM_DURATION = 500;

    int numCols = ceil(sqrt(num));
    int numRows = ceil((float)num / (float)numCols);

    int r = 0, c = 0;
    for (SurfaceTransform *surf: m_surfaces) {
        if (!surf->surface->isMapped() || surf->surface->workspace() != currWs) {
            continue;
        }

        if (m_scaled) {
            surf->ss = surf->cs;
            surf->ts = 1.f;

            surf->sx = surf->cx;
            surf->sy = surf->cy;
            surf->tx = surf->ty = 0.f;

            surf->animation.setStart(0.f);
            surf->animation.setTarget(1.f);
            surf->animation.run(surf->surface->output(), ANIM_DURATION, Animation::Flags::SendDone);

            surf->alphaAnim.setStart(surf->surface->alpha());
            surf->alphaAnim.setTarget(1.f);
            surf->alphaAnim.run(surf->surface->output(), ALPHA_ANIM_DURATION);
        } else {
            int cellW = surf->surface->output()->width / numCols;
            int cellH = surf->surface->output()->height / numRows;

            float rx = (float)cellW / (float)surf->surface->transformedWidth();
            float ry = (float)cellH / (float)surf->surface->transformedHeight();
            if (rx > ry) {
                rx = ry;
            } else {
                ry = rx;
            }
            int x = c * cellW - surf->surface->x() + (cellW - (surf->surface->transformedWidth() * rx)) / 2.f;
            int y = r * cellH - surf->surface->y() + (cellH - (surf->surface->transformedHeight() * ry)) / 2.f;

            struct weston_matrix *matrix = &surf->transform.matrix;
            weston_matrix_init(matrix);
            weston_matrix_scale(matrix, surf->cs, surf->cs, 1.f);
            weston_matrix_translate(matrix, surf->cx, surf->cy, 0);

            surf->ss = surf->cs;
            surf->sx = surf->cx;
            surf->sy = surf->cy;

            surf->ts = rx * surf->cs;
            surf->tx = x;
            surf->ty = y;

            surf->animation.setStart(0.f);
            surf->animation.setTarget(1.f);
            surf->animation.run(surf->surface->output(), ANIM_DURATION);

            surf->alphaAnim.setStart(surf->surface->alpha());
            surf->alphaAnim.setTarget(INACTIVE_ALPHA);
            surf->alphaAnim.run(surf->surface->output(), ALPHA_ANIM_DURATION);

            surf->surface->addTransform(&surf->transform);
        }
        if (++c >= numCols) {
            c = 0;
            ++r;
        }
    }
    m_scaled = !m_scaled;
    if (m_scaled) {
        m_seat = ws;
        shell()->startGrab(m_grab, &grab_interface, ws, DESKTOP_SHELL_CURSOR_ARROW);
        shell()->hidePanels();
        if (ws->pointer.current) {
            ShellSurface *s = Shell::getShellSurface(container_of(ws->pointer.current, struct weston_surface, surface));
            if (!s) {
                return;
            }

            for (SurfaceTransform *tr: m_surfaces) {
                if (tr->surface == s) {
                    tr->alphaAnim.setStart(tr->surface->alpha());
                    tr->alphaAnim.setTarget(1.0);
                    tr->alphaAnim.run(tr->surface->output(), ALPHA_ANIM_DURATION);
                    break;
                }
            }
        }
    } else {
        m_seat = nullptr;
        Shell::endGrab(m_grab);
        shell()->showPanels();
    }
}

void ScaleEffect::end(ShellSurface *surface)
{
    shell()->activateSurface(surface, m_seat);
    run(m_seat);
}

void ScaleEffect::addedSurface(ShellSurface *surface)
{
    SurfaceTransform *tr = new SurfaceTransform;
    tr->surface = surface;
    tr->animation.updateSignal.connect(tr, &SurfaceTransform::updateAnimation);
    tr->animation.doneSignal.connect(tr, &SurfaceTransform::doneAnimation);
    tr->alphaAnim.updateSignal.connect(surface, &ShellSurface::setAlpha);
    tr->animation.setCurve(OutElasticCurve());

    wl_list_init(&tr->transform.link);

    tr->cx = tr->cy = 0;
    tr->cs = 1.f;

    m_surfaces.push_back(tr);

    if (m_scaled) {
        m_scaled = false;
        run(m_seat);
    }
}

void ScaleEffect::removedSurface(ShellSurface *surface)
{
    for (auto i = m_surfaces.begin(); i != m_surfaces.end(); ++i) {
        if ((*i)->surface == surface) {
            delete *i;
            m_surfaces.erase(i);
            break;
        }
    }

    if (m_scaled) {
        if (!m_surfaces.empty()) {
            m_scaled = false;
        }
        run(m_seat);
    }
}

void SurfaceTransform::updateAnimation(float value)
{
    struct weston_matrix *matrix = &transform.matrix;
    weston_matrix_init(matrix);
    cs = ss + (ts - ss) * value;
    weston_matrix_scale(matrix, cs, cs, 1.f);

    cx = sx + (float)(tx - sx) * value;
    cy = sy + (float)(ty - sy) * value;
    weston_matrix_translate(matrix, cx, cy, 0);
    surface->damage();
}

void SurfaceTransform::doneAnimation()
{
    surface->removeTransform(&transform);
}
