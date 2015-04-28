/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#include "utils.h"

namespace Utils {

/*!
 * \brief Converts \p name from 'key-name' notation
 *        to camel case 'keyName' or 'KeyName'.
 * This function converts GSettings key names to names
 * suitable for Qt getters and setters.
 *
 * \param name key name
 * \return key name converted to camel case
 * \internal
 */
QString toCamelCase(const gchar *name)
{
    QString result;
    bool nextUpperCase = false;

    while (*name) {
        if (*name == '-') {
            // Characters following a dash are upper case
            nextUpperCase = true;
        } else if (nextUpperCase) {
            result.append(QChar(*name).toUpper());
            nextUpperCase = false;
        } else {
            result.append(*name);
        }

        name++;
    }

    return result;
}

/*!
 * \brief Converts \p name from camel case notation
 *        such as 'keyName' or 'KeyName' to the form 'key-name'.
 * This function converts Qt camel case key names to a
 * format suitable for GSettings.
 *
 * The result of this function must be freed by
 * the caller.
 *
 * \param name key name
 * \return key name converted from camel case
 * \internal
 */
gchar *fromCamelCase(const QString &name)
{
    GString *string = g_string_new(Q_NULLPTR);

    QByteArray bytes = name.toLatin1();
    for (int i = 0; i < bytes.size(); i++) {
        QChar c(bytes.at(i));
        if (c.isUpper()) {
            g_string_append_c(string, '-');
            g_string_append_c(string, c.toLower().toLatin1());
        } else {
            g_string_append_c(string, c.toLatin1());
        }
    }

    return g_string_free(string, false);
}

}
