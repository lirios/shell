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
    wl_list_init(&m_layer.link);
}

void Layer::insert(struct weston_layer *below)
{
    if (below) {
        wl_list_remove(&m_layer.link);
        wl_list_insert(&below->link, &m_layer.link);
        for (weston_view *v: *this) {
            weston_surface_damage(v->surface);
        }
    }
}

void Layer::insert(Layer *below)
{
    if (below) {
        wl_list_remove(&m_layer.link);
        wl_list_insert(&below->m_layer.link, &m_layer.link);
        for (weston_view *v: *this) {
            weston_surface_damage(v->surface);
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
    for (weston_view *v: *this) {
        weston_view_damage_below(v);
        weston_surface_schedule_repaint(v->surface);
    }
    if (!wl_list_empty(&m_layer.link)) {
        m_below = m_layer.link.prev;
        wl_list_remove(&m_layer.link);
        wl_list_init(&m_layer.link);
    }
}

bool Layer::isVisible() const
{
    return !wl_list_empty(&m_layer.link) && !wl_list_empty(&m_layer.view_list);
}

void Layer::show()
{
    if (m_below) {
        wl_list_insert(m_below, &m_layer.link);
    }
    for (weston_view *v: *this) {
        weston_surface_damage(v->surface);
    }
}

void Layer::addSurface(weston_view *view)
{
    if (view->layer_link.prev) {
        wl_list_remove(&view->layer_link);
    }
    wl_list_insert(&m_layer.view_list, &view->layer_link);
}

void Layer::addSurface(ShellSurface *surf)
{
    addSurface(surf->view());
}

void Layer::stackAbove(weston_view *surf, weston_view *parent)
{
    wl_list_remove(&surf->layer_link);
    wl_list_init(&surf->layer_link);

    wl_list_insert(parent->layer_link.prev, &surf->layer_link);
}

void Layer::stackBelow(weston_view *surf, weston_view *parent)
{
    wl_list_remove(&surf->layer_link);
    wl_list_init(&surf->layer_link);

    wl_list_insert(parent->layer_link.prev->prev, &surf->layer_link);
}

void Layer::restack(weston_view *view)
{
    wl_list_remove(&view->layer_link);
    wl_list_insert(&m_layer.view_list, &view->layer_link);
    weston_view_damage_below(view);
    weston_surface_damage(view->surface);
}

void Layer::restack(ShellSurface *surf)
{
    restack(surf->view());
}

bool Layer::isEmpty() const
{
    return wl_list_empty(&m_layer.view_list);
}

int Layer::numberOfSurfaces() const
{
    return wl_list_length(&m_layer.view_list);
}

Layer::iterator Layer::begin() const
{
    return iterator(&m_layer.view_list, m_layer.view_list.next, false);
}

Layer::iterator Layer::rbegin() const
{
    return iterator(&m_layer.view_list, m_layer.view_list.prev, true);
}

Layer::iterator Layer::end() const
{
    return iterator(&m_layer.view_list, &m_layer.view_list, false);
}
