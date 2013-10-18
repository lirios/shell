/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
  *
 * Author(s):
 *    Giulio Camuffo
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

#ifndef WESTONVERSION_H
#define WESTONVERSION_H

#include <weston/version.h>

#define WESTON_VERSION_NUMBER ((WESTON_VERSION_MAJOR << 16) | (WESTON_VERSION_MINOR << 8) | (WESTON_VERSION_MICRO))

/*
    Can be used like #if (WESTON_VERSION_NUMBER >= WESTON_VERSION_CHECK(1, 2, 0))
*/
#define WESTON_VERSION_CHECK(major, minor, micro) ((major << 16) | (minor << 8) | (micro))


#endif // WESTONVERSION_H
