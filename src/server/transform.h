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

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <weston/compositor.h>

class Transform {
public:
    Transform();

    void reset();
    void scale(float x, float y, float z);
    void translate(float x, float y, float z);

    inline const struct weston_transform *nativeHandle() const { return &m_transform; }
    inline struct weston_transform *nativeHandle() { return &m_transform; }

private:
    struct weston_transform m_transform;
};

#endif
