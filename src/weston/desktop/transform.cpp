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

#include "transform.h"

#include <weston/matrix.h>

Transform::Transform()
{
    wl_list_init(&m_transform.link);
    reset();
}

void Transform::reset()
{
    weston_matrix_init(&m_transform.matrix);
}

void Transform::scale(float x, float y, float z)
{
    weston_matrix_scale(&m_transform.matrix, x, y, z);
}

void Transform::translate(float x, float y, float z)
{
    weston_matrix_translate(&m_transform.matrix, x, y, z);
}
