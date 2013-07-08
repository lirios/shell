/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2013 Giulio Camuffo <giuliocamuffo@gmail.com>
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

#include "zoomeffect.h"
#include "shell.h"
#include "utils.h"

ZoomEffect::ZoomEffect(Shell *shell)
    : Effect(shell)
{
    m_binding = shell->bindAxis(WL_POINTER_AXIS_VERTICAL_SCROLL, MODIFIER_SUPER, &ZoomEffect::run, this);
}

ZoomEffect::~ZoomEffect()
{
    delete m_binding;
}

void ZoomEffect::run(struct wl_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value)
{
    struct weston_seat *ws = wl_seat2weston_seat(seat);
    struct weston_compositor *compositor = ws->compositor;
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
                output->zoom.active = 1;
                output->disable_planes++;
            }

            output->zoom.spring_z.target = output->zoom.level;
            weston_output_update_zoom(output, output->zoom.type);
        }
    }
}
