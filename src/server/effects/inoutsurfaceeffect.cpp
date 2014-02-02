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

#include "inoutsurfaceeffect.h"
#include "animation.h"
#include "shellsurface.h"

const int ALPHA_ANIM_DURATION = 200;

struct InOutSurfaceEffect::Surface {
    weston_view *view;
    Animation animation;
    InOutSurfaceEffect *effect;
    struct Listener {
        struct wl_listener destroyListener;
        Surface *parent;
    } listener;

    void setAlpha(float alpha)
    {
        view->alpha = alpha;
        weston_surface_damage(view->surface);
    }
    void done()
    {
        weston_surface_destroy(view->surface);
        effect->m_surfaces.remove(this);
        delete this;
    }
    static void destroyed(struct wl_listener *listener, void *data)
    {
        Surface *surf = container_of(listener, Listener, destroyListener)->parent;

        surf->animation.setStart(surf->view->alpha);
        surf->animation.setTarget(0);
        surf->animation.run(surf->view->output, ALPHA_ANIM_DURATION, Animation::Flags::SendDone);
    }
};

InOutSurfaceEffect::InOutSurfaceEffect()
                  : Effect()
{
}

InOutSurfaceEffect::~InOutSurfaceEffect()
{
    while (!m_surfaces.empty()) {
        Surface *s = m_surfaces.front();
        weston_surface_destroy(s->view->surface);
        delete s;
        m_surfaces.pop_front();
    }
}

void InOutSurfaceEffect::addedSurface(ShellSurface *surface)
{
    Surface *surf = new Surface;
    surf->view = surface->view();
    surf->effect = this;

    ++surface->weston_surface()->ref_count;
    surf->listener.parent = surf;
    surf->listener.destroyListener.notify = Surface::destroyed;
    wl_resource_add_destroy_listener(surf->view->surface->resource, &surf->listener.destroyListener);

    surf->animation.updateSignal->connect(surf, &Surface::setAlpha);
    surf->animation.doneSignal->connect(surf, &Surface::done);
    m_surfaces.push_back(surf);

    surf->animation.setStart(0);
    surf->animation.setTarget(1);
    surf->animation.run(surface->output(), ALPHA_ANIM_DURATION);
}



InOutSurfaceEffect::Settings::Settings()
           : Effect::Settings()
           , m_effect(nullptr)
{
}

InOutSurfaceEffect::Settings::~Settings()
{
    delete m_effect;
}

void InOutSurfaceEffect::Settings::set(const std::string &name, int v)
{
    if (name == "enabled") {
        if (v && !m_effect) {
            m_effect = new InOutSurfaceEffect;
        } else if (!v) {
            delete m_effect;
            m_effect = nullptr;
        }
    }
}

void InOutSurfaceEffect::Settings::unSet(const std::string &name)
{
    if (name == "enabled") {
        delete m_effect;
        m_effect = nullptr;
    }
}

SETTINGS(inoutsurface_effect, InOutSurfaceEffect::Settings)
