/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include <HawaiiShell/Package>

namespace Hawaii {

namespace Shell {

class PluginLoaderPrivate;

class HAWAIISHELL_EXPORT PluginLoader
{
public:
    PluginLoader();
    ~PluginLoader();

    static PluginLoader *instance();

    Package loadPackage(const QString &format);

private:
    Q_DECLARE_PRIVATE(PluginLoader)
    PluginLoaderPrivate *const d_ptr;
};

} // namespace Shell

} // namespace Hawaii

#endif // PLUGINLOADER_H
