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

#include "workspace.h"
#include "shell.h"
#include "shellsurface.h"
#include "utils.h"

Workspace::Workspace(Shell *shell, int number)
         : m_shell(shell)
         , m_number(number)
         , m_background(nullptr)
         , m_active(false)
{
    int x = 0, y = 0;
    int w = 0, h = 0;

    weston_surface *s = weston_surface_create(shell->compositor());
    m_rootSurface = weston_view_create(s);
    s->configure = [](struct weston_surface *es, int32_t sx, int32_t sy) {};
    s->configure_private = 0;
    weston_view_set_position(m_rootSurface, x, y);
    s->width = w;
    s->height = h;
    weston_surface_set_color(s, 0.0, 0.0, 0.0, 1);
    pixman_region32_fini(&s->opaque);
    pixman_region32_init_rect(&s->opaque, 0, 0, w, h);
    pixman_region32_fini(&s->input);
    pixman_region32_init_rect(&s->input, 0, 0, w, h);

    m_layer.addSurface(m_rootSurface);

    m_backgroundDestroy.signal->connect(this, &Workspace::backgroundDestroyed);
}

Workspace::~Workspace()
{
    for (weston_view *v: m_layer) {
        ShellSurface *shsurf = Shell::getShellSurface(v->surface);
        if (!shsurf)
            continue;

        if (shsurf->transformParent() == m_rootSurface) {
            weston_view_set_transform_parent(v, nullptr);
        }
    }

    remove();
    destroyedSignal(this);
    weston_view_destroy(m_background);
    weston_surface_destroy(m_rootSurface->surface);
}

void Workspace::createBackgroundView(weston_surface *bkg)
{
    if (m_background && m_background->surface != bkg) {
        weston_view_destroy(m_background);
    }

    m_background = weston_view_create(bkg);
    m_backgroundDestroy.listen(&m_background->destroy_signal);
    weston_view_set_position(m_background, 0, 0);
    m_backgroundLayer.addSurface(m_background);
    weston_view_set_transform_parent(m_background, m_rootSurface);
}

void Workspace::backgroundDestroyed(void *d)
{
    if (d == m_background) {
        m_background = nullptr;
        m_backgroundDestroy.reset();
    }
}

void Workspace::addSurface(ShellSurface *surface)
{
    if (!surface->transformParent()) {
        weston_view_set_transform_parent(surface->view(), m_rootSurface);
    }
    m_layer.addSurface(surface);
    surface->m_workspace = this;
}

void Workspace::removeSurface(ShellSurface *surface)
{
    if (surface->transformParent() == m_rootSurface) {
        weston_view_set_transform_parent(surface->view(), nullptr);
    }
    wl_list_remove(&surface->view()->layer_link);
    surface->m_workspace = nullptr;
}

void Workspace::restack(ShellSurface *surface)
{
    m_layer.restack(surface);
}

void Workspace::stackAbove(weston_view *surf, weston_view *parent)
{
    m_layer.stackAbove(surf, parent);
}

void Workspace::setTransform(const Transform &tr)
{
    wl_list_remove(&m_transform.nativeHandle()->link);
    m_transform = tr;
    wl_list_insert(&m_rootSurface->geometry.transformation_list, &m_transform.nativeHandle()->link);

    weston_view_geometry_dirty(m_rootSurface);
    weston_surface_damage(m_rootSurface->surface);
}

IRect2D Workspace::boundingBox() const
{
    pixman_box32_t *box = pixman_region32_extents(&m_background->transform.boundingbox);
    IRect2D rect(box->x1, box->y1, box->x2 - box->x1, box->y2 - box->y1);

    return rect;
}

int Workspace::numberOfSurfaces() const
{
    return m_layer.numberOfSurfaces();
}

struct weston_output *Workspace::output() const
{
    return m_shell->getDefaultOutput();
    return m_rootSurface->output;
}

void Workspace::insert(Workspace *ws)
{
    m_layer.insert(&ws->m_layer);
    m_backgroundLayer.insert(&m_layer);
}

void Workspace::insert(Layer *layer)
{
    m_layer.insert(layer);
    m_backgroundLayer.insert(&m_layer);
}

void Workspace::insert(struct weston_layer *layer)
{
    m_layer.insert(layer);
    m_backgroundLayer.insert(&m_layer);
}

void Workspace::remove()
{
    m_layer.remove();
}

void Workspace::setActive(bool active)
{
    m_active = active;
    activeChangedSignal();
}
