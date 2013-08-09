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

Workspace::Workspace(Shell *shell, int number)
         : m_shell(shell)
         , m_number(number)
{
    int x = 0, y = 0;
    int w = 0, h = 0;

    m_rootSurface = weston_surface_create(shell->compositor());
    m_rootSurface->configure = [](struct weston_surface *es, int32_t sx, int32_t sy, int32_t width, int32_t height) {};
    m_rootSurface->configure_private = 0;
    weston_surface_configure(m_rootSurface, x, y, w, h);
    weston_surface_set_color(m_rootSurface, 0.0, 0.0, 0.0, 1);
    pixman_region32_fini(&m_rootSurface->opaque);
    pixman_region32_init_rect(&m_rootSurface->opaque, 0, 0, w, h);
    pixman_region32_fini(&m_rootSurface->input);
    pixman_region32_init_rect(&m_rootSurface->input, 0, 0, w, h);

    m_layer.addSurface(m_rootSurface);
}

Workspace::~Workspace()
{

}

void Workspace::addSurface(ShellSurface *surface)
{
    if (!surface->transformParent()) {
        weston_surface_set_transform_parent(surface->m_surface, m_rootSurface);
    }
    m_layer.addSurface(surface);
}

void Workspace::restack(ShellSurface *surface)
{
    m_layer.restack(surface);
}

void Workspace::stackAbove(struct weston_surface *surf, struct weston_surface *parent)
{
    m_layer.stackAbove(surf, parent);
}

void Workspace::setTransform(const Transform &tr)
{
    wl_list_remove(&m_transform.nativeHandle()->link);
    m_transform = tr;
    wl_list_insert(&m_rootSurface->geometry.transformation_list, &m_transform.nativeHandle()->link);

    weston_surface_geometry_dirty(m_rootSurface);
    weston_surface_damage(m_rootSurface);
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
}

void Workspace::insert(Layer *layer)
{
    m_layer.insert(layer);
}

void Workspace::insert(struct weston_layer *layer)
{
    m_layer.insert(layer);
}

void Workspace::remove()
{
    m_layer.remove();
}
