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

#ifndef ZOOMEFFECT_H
#define ZOOMEFFECT_H

#include "effect.h"

class Binding;

class ZoomEffect : public Effect {
public:
    ZoomEffect(Shell *shell);
    ~ZoomEffect();

private:
    void run(struct wl_seat *seat, uint32_t time, uint32_t axis, wl_fixed_t value);

    Binding *m_binding;
};

#endif
