/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013-2014 Giulio Camuffo <giuliocamuffo@gmail.com>
 *
 * Author(s):
 *    Giulio Camuffo
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

#include "minimizeeffect.h"
#include "animation.h"
#include "shellsurface.h"

static const int ANIM_DURATION = 150;

struct MinimizeEffect::Surface {
    ShellSurface *surface;
    Animation animation;
    weston_transform transform;
    float targetY;
    bool minimizing;

    void minimized(ShellSurface *surf)
    {
        minimizing = true;
        surf->show();

        struct weston_matrix *matrix = &transform.matrix;
        weston_matrix_init(matrix);
        weston_matrix_scale(matrix, 1, 1, 1.f);
        targetY = surf->transformedHeight() / 2.f;
        surf->addTransform(&transform);

        animation.setStart(1);
        animation.setTarget(0.01);
        animation.run(surf->output(), ANIM_DURATION, Animation::Flags::SendDone);

    }
    void unminimized(ShellSurface *surf)
    {
        minimizing = false;
        struct weston_matrix *matrix = &transform.matrix;
        weston_matrix_init(matrix);
        weston_matrix_scale(matrix, 1, 0.01, 1.f);
        targetY = surf->height() / 2.f;

        surf->addTransform(&transform);

        animation.setStart(0.01);
        animation.setTarget(1);
        animation.run(surf->output(), ANIM_DURATION, Animation::Flags::SendDone);
    }
    void animate(float value)
    {
        struct weston_matrix *matrix = &transform.matrix;
        weston_matrix_init(matrix);
        weston_matrix_scale(matrix, 1, value, 1.f);
        weston_matrix_translate(matrix, 0, targetY * (1.f - value), 0);

        surface->damage();

    }
    void done()
    {
        if (minimizing) {
            surface->hide();
        }
        surface->removeTransform(&transform);
    }
};

MinimizeEffect::MinimizeEffect(Shell *shell)
              : Effect(shell)
{
}

MinimizeEffect::~MinimizeEffect()
{
    for (auto i = m_surfaces.begin(); i != m_surfaces.end(); ++i) {
        Surface *s = *i;
        s->surface->minimizedSignal.disconnect(s);
        delete *i;
        m_surfaces.erase(i);
    }
}

void MinimizeEffect::addedSurface(ShellSurface *surface)
{
    Surface *surf = new Surface;
    surf->surface = surface;
    wl_list_init(&surf->transform.link);

    surface->minimizedSignal.connect(surf, &Surface::minimized);
    surface->unminimizedSignal.connect(surf, &Surface::unminimized);

    surf->animation.updateSignal->connect(surf, &Surface::animate);
    surf->animation.doneSignal->connect(surf, &Surface::done);
    m_surfaces.push_back(surf);
}

void MinimizeEffect::removedSurface(ShellSurface *surface)
{
    for (auto i = m_surfaces.begin(); i != m_surfaces.end(); ++i) {
        Surface *s = *i;
        if (s->surface == surface) {
            surface->minimizedSignal.disconnect(s);
            delete *i;
            m_surfaces.erase(i);
            break;
        }
    }
}
