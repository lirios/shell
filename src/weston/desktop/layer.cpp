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

#include "layer.h"
#include "shellsurface.h"

Layer::Layer()
     : m_below(nullptr)
{
    weston_layer_init(&m_layer, nullptr);
}

void Layer::insert(struct weston_layer *below)
{
    if (below) {
        wl_list_insert(&below->link, &m_layer.link);
        for (struct weston_surface *s: *this) {
            weston_surface_damage(s);
        }
    }
}

void Layer::insert(Layer *below)
{
    if (below) {
        wl_list_insert(&below->m_layer.link, &m_layer.link);
        for (struct weston_surface *s: *this) {
            weston_surface_damage(s);
        }
    }
}

void Layer::remove()
{
    hide();
    m_below = nullptr;
}

void Layer::hide()
{
    for (struct weston_surface *s: *this) {
        weston_surface_damage_below(s);
        weston_surface_schedule_repaint(s);
    }
    if (!wl_list_empty(&m_layer.link)) {
        m_below = m_layer.link.prev;
        wl_list_remove(&m_layer.link);
        wl_list_init(&m_layer.link);
    }
}

bool Layer::isVisible() const
{
    return !wl_list_empty(&m_layer.link) && !wl_list_empty(&m_layer.surface_list);
}

void Layer::show()
{
    if (m_below) {
        wl_list_insert(m_below, &m_layer.link);
    }
    for (struct weston_surface *s: *this) {
        weston_surface_damage(s);
    }
}

void Layer::addSurface(struct weston_surface *surf)
{
    if (surf->layer_link.prev) {
        wl_list_remove(&surf->layer_link);
    }
    wl_list_insert(&m_layer.surface_list, &surf->layer_link);
}

void Layer::addSurface(ShellSurface *surf)
{
    addSurface(surf->m_surface);
}

void Layer::stackAbove(struct weston_surface *surf, struct weston_surface *parent)
{
    wl_list_remove(&surf->layer_link);
    wl_list_init(&surf->layer_link);

    wl_list_insert(parent->layer_link.prev, &surf->layer_link);
}

void Layer::stackBelow(struct weston_surface *surf, struct weston_surface *parent)
{
    wl_list_remove(&surf->layer_link);
    wl_list_init(&surf->layer_link);

    wl_list_insert(parent->layer_link.prev->prev, &surf->layer_link);
}

void Layer::restack(struct weston_surface *surf)
{
    weston_surface_restack(surf, &m_layer.surface_list);
}

void Layer::restack(ShellSurface *surf)
{
    restack(surf->m_surface);
}

bool Layer::isEmpty() const
{
    return wl_list_empty(&m_layer.surface_list);
}

int Layer::numberOfSurfaces() const
{
    return wl_list_length(&m_layer.surface_list);
}

Layer::iterator Layer::begin()
{
    return iterator(&m_layer.surface_list, m_layer.surface_list.next);
}

Layer::const_iterator Layer::begin() const
{
    return const_iterator(&m_layer.surface_list, m_layer.surface_list.next);
}

Layer::iterator Layer::end()
{
    return iterator(&m_layer.surface_list, &m_layer.surface_list);
}

Layer::const_iterator Layer::end() const
{
    return const_iterator(&m_layer.surface_list, &m_layer.surface_list);
}
