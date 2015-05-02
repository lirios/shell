/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef QGVARIANTUTILS_H
#define QGVARIANTUTILS_H

#include <QtCore/QVariant>

extern "C" {
#include <glib.h>
}

namespace Utils {

QVariant toQVariant(GVariant *value);
GVariant *toGVariant(const GVariantType *type, const QVariant &variant);
}

#endif // QGVARIANTUTILS_H
