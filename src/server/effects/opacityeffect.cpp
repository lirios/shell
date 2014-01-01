/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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

#include "opacityeffect.h"
#include "shell.h"
#include "shellsurface.h"
#include "utils.h"

OpacityEffect::OpacityEffect(Shell *shell)
    : Effect(shell)
{
    m_binding = shell->bindAxis(WL_POINTER_AXIS_VERTICAL_SCROLL,
                                (weston_keyboard_modifier)(MODIFIER_SUPER | MODIFIER_ALT),
                                &OpacityEffect::run, this);
}

OpacityEffect::~OpacityEffect()
{
    delete m_binding;
}

void OpacityEffect::run(struct weston_seat *seat, uint32_t time,
                        uint32_t axis, wl_fixed_t value)
{
    float step = 0.005;

    weston_surface *surface = weston_surface_get_main_surface(seat->pointer->focus);
    if (!surface)
        return;

    ShellSurface *shsurf = shell()->getShellSurface(surface);
    if (!shsurf)
        return;

    double newvalue = wl_fixed_to_double(value) * step;

    if (surface->alpha != surface->alpha - newvalue) {
        // Set surface alpha
        surface->alpha -= newvalue;

        // Alpha in [0.05:1.0] range
        if (surface->alpha > 1.f)
            surface->alpha = 1.f;
        if (surface->alpha < 0.05f)
            surface->alpha = 0.05f;

        // Redraw
        weston_surface_geometry_dirty(surface);
        weston_surface_damage(surface);
    }

    // Other effects can't go beyond this alpha value
    shsurf->setMaximumAlpha(surface->alpha);
}
