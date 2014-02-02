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

#include "zoomeffect.h"
#include "shell.h"
#include "utils.h"
#include "binding.h"

ZoomEffect::ZoomEffect()
          : Effect()
{
    Binding *b = new Binding();
    b->axisTriggered.connect(this, &ZoomEffect::run);
    addBinding("Zoom", b);
}

ZoomEffect::~ZoomEffect()
{
}

void ZoomEffect::run(struct weston_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    struct weston_compositor *compositor = seat->compositor;
    struct weston_output *output;

    wl_list_for_each(output, &compositor->output_list, link) {
        if (pixman_region32_contains_point(&output->region, wl_fixed_to_double(seat->pointer->x),
                                           wl_fixed_to_double(seat->pointer->y), nullptr)) {
            /* For every pixel zoom 20th of a step */
            float increment = output->zoom.increment * -wl_fixed_to_double(value) / 20.f;

            output->zoom.level += increment;

            if (output->zoom.level < 0.f)
                output->zoom.level = 0.f;
            else if (output->zoom.level > output->zoom.max_level)
                output->zoom.level = output->zoom.max_level;
            else if (!output->zoom.active) {
                weston_output_activate_zoom(output);
            }

            output->zoom.spring_z.target = output->zoom.level;
            weston_output_update_zoom(output);
        }
    }
}



ZoomEffect::Settings::Settings()
           : Effect::Settings()
           , m_effect(nullptr)
{
}

ZoomEffect::Settings::~Settings()
{
    delete m_effect;
}

std::list<Option> ZoomEffect::Settings::options() const
{
    auto list = Effect::Settings::options();
    list.push_back(Option::binding("zoom_binding", Binding::Type::Axis));

    return list;
}

void ZoomEffect::Settings::unSet(const std::string &name)
{
    if (name == "enabled") {
        delete m_effect;
        m_effect = nullptr;
    } else if (name == "zoom_binding") {
        m_effect->binding("Zoom")->reset();
    }
}

void ZoomEffect::Settings::set(const std::string &name, int v)
{
    if (name == "enabled") {
        if (v && !m_effect) {
            m_effect = new ZoomEffect;
            const Option *o = option("zoom_binding");
            o->valueAsBinding().bind(m_effect->binding("Zoom"));
        } else if (!v) {
            delete m_effect;
            m_effect = nullptr;
        }
    }
}

void ZoomEffect::Settings::set(const std::string &name, const Option::BindingValue &v)
{
    if (name == "zoom_binding" && m_effect) {
        v.bind(m_effect->binding("Zoom"));
    }
}

SETTINGS(zoom_effect, ZoomEffect::Settings)
