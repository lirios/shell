/*
 * Copyright 2013-2014  Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include <weston/compositor.h>

#include "dropdown.h"
#include "utils.h"
#include "shell.h"
#include "binding.h"
#include "animation.h"
#include "wayland-dropdown-server-protocol.h"

class Instance
{
public:
    Instance(Dropdown *dd, wl_resource *res)
        : m_dropdown(dd)
        , m_resource(res)
        , m_visible(false)
        , m_animValue(1.f)
    {
        wl_resource_set_implementation(m_resource, &s_implementation, this, [](wl_resource *res) {
            Instance *in = static_cast<Instance *>(wl_resource_get_user_data(res));
            in->m_dropdown->m_instances.remove(wl_resource_get_client(res));
            delete in;
        });

        m_toggleBinding.keyTriggered.connect(this, &Instance::toggle);
        m_toggleBinding.bindKey(KEY_F11, (weston_keyboard_modifier)0);
        m_animation.updateSignal->connect(this, &Instance::updateAnim);

        m_surfaceListener.signal->connect(this, &Instance::surfaceDestroyed);
    }

    ~Instance()
    {
        m_surfaceListener.reset();
        if (m_view) {
            weston_view_destroy(m_view);
        }
    }

    void surfaceDestroyed(void *d)
    {
        m_surfaceListener.reset();
        m_view = nullptr;
        wl_resource_destroy(m_resource);
    }

    void setSurface(wl_client *client, wl_resource *resource, wl_resource *outResource, wl_resource *surfResource)
    {
        weston_surface *surface = static_cast<weston_surface *>(wl_resource_get_user_data(surfResource));
        m_output = static_cast<weston_output *>(wl_resource_get_user_data(outResource));

        surface->configure = [](weston_surface *es, int32_t sx, int32_t sy) {
            static_cast<Instance *>(es->configure_private)->configure(es, sx, sy);
        };
        surface->configure_private = this;
        surface->output = m_output;
        m_surfaceListener.listen(&surface->destroy_signal);

        m_view = weston_view_create(surface);
        setPos();
        Shell::instance()->addStickyView(m_view);
    }

    void configure(weston_surface *es, int32_t sx, int32_t sy)
    {
        setPos();
    }

    void toggle(weston_seat *seat, uint32_t time, uint32_t key)
    {
        m_visible = !m_visible;
        if (m_visible) {
            weston_surface_activate(m_view->surface, seat);
        }

        m_animation.setStart(m_animValue);
        m_animation.setTarget(!m_visible);
        m_animation.run(m_output, 200);
    }

    void updateAnim(float value)
    {
        m_animValue = value;
        setPos();
    }

    void setPos()
    {
        const IRect2D &available = Shell::instance()->windowsArea(m_output);
        int x = available.x + (available.width - m_view->surface->width) / 2.f;
        int y = available.y - m_animValue * (m_view->surface->height + available.y - m_output->y);
        weston_view_set_position(m_view, x, y);
    }

    Dropdown *m_dropdown;
    wl_resource *m_resource;
    Binding m_toggleBinding;
    bool m_visible;
    weston_output *m_output;
    weston_view *m_view;
    Animation m_animation;
    float m_animValue;
    WlListener m_surfaceListener;
    static const struct nuclear_dropdown_interface s_implementation;
};

const struct nuclear_dropdown_interface Instance::s_implementation = {
    wrapInterface(&Instance::setSurface)
};

Dropdown::Dropdown()
          : Interface()
{
    wl_global_create(Shell::instance()->compositor()->wl_display, &nuclear_dropdown_interface, 1, this,
                     [](wl_client *client, void *data, uint32_t version, uint32_t id) {
                         static_cast<Dropdown *>(data)->bind(client, version, id);
                     });

}

std::list<wl_client *> Dropdown::boundClients() const
{
    return m_instances;
}

void Dropdown::bind(wl_client *client, uint32_t version, uint32_t id)
{
    new Instance(this, wl_resource_create(client, &nuclear_dropdown_interface, version, id));
    m_instances.push_back(client);
}
