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

#include "fademovingeffect.h"
#include "animation.h"
#include "shellsurface.h"

const int ALPHA_ANIM_DURATION = 200;

struct FadeMovingEffect::Surface {
    ShellSurface *surface;
    Animation animation;
};

FadeMovingEffect::FadeMovingEffect(Shell *shell)
                : Effect(shell)
{
}

FadeMovingEffect::~FadeMovingEffect()
{
    for (Surface *s: m_surfaces) {
        s->surface->moveStartSignal.disconnect(this, &FadeMovingEffect::start);
        s->surface->moveEndSignal.disconnect(this, &FadeMovingEffect::end);
        delete s;
    }
}

void FadeMovingEffect::start(ShellSurface *surface)
{
    Surface *surf = findSurface(surface);
    surf->animation.setStart(surface->alpha());
    surf->animation.setTarget(0.8);
    surf->animation.run(surface->output(), ALPHA_ANIM_DURATION);
}

void FadeMovingEffect::end(ShellSurface *surface)
{
    Surface *surf = findSurface(surface);
    surf->animation.setStart(surface->alpha());
    surf->animation.setTarget(1.0);
    surf->animation.run(surface->output(), ALPHA_ANIM_DURATION);
}

void FadeMovingEffect::addedSurface(ShellSurface *surface)
{
    Surface *surf = new Surface;
    surf->surface = surface;

    surface->moveStartSignal.connect(this, &FadeMovingEffect::start);
    surface->moveEndSignal.connect(this, &FadeMovingEffect::end);
    surf->animation.updateSignal.connect(surface, &ShellSurface::setAlpha);

    m_surfaces.push_back(surf);
}

void FadeMovingEffect::removedSurface(ShellSurface *surface)
{
    surface->moveStartSignal.disconnect(this, &FadeMovingEffect::start);
    surface->moveEndSignal.disconnect(this, &FadeMovingEffect::end);

    for (auto i = m_surfaces.begin(); i != m_surfaces.end(); ++i) {
        if ((*i)->surface == surface) {
            delete *i;
            m_surfaces.erase(i);
            break;
        }
    }
}

FadeMovingEffect::Surface *FadeMovingEffect::findSurface(ShellSurface *surface)
{
    for (Surface *s: m_surfaces) {
        if (s->surface == surface) {
            return s;
        }
    }

    return nullptr;
}
