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

#include "packages_p.h"
#include "pluginloader.h"
#include "pluginloader_p.h"

namespace Hawaii {

namespace Shell {

Q_GLOBAL_STATIC(PluginLoader, s_pluginLoader)

/*
 * PluginLoaderPrivate
 */

PluginLoaderPrivate::PluginLoaderPrivate()
{
}

PluginLoaderPrivate::~PluginLoaderPrivate()
{
    for (PackageStructurePtr ptr: structures)
        delete ptr.data();
}

/*
 * PluginLoader
 */

PluginLoader::PluginLoader()
    : d_ptr(new PluginLoaderPrivate())
{
}

PluginLoader::~PluginLoader()
{
    delete d_ptr;
}

PluginLoader *PluginLoader::instance()
{
    return s_pluginLoader();
}

Package PluginLoader::loadPackage(const QString &format)
{
    Q_D(PluginLoader);

    if (format.isEmpty())
        return Package();

    // Return a previously created package structure for this format
    PackageStructure *structure = d->structures.value(format).data();
    if (structure)
        return Package(structure);

    // Create a package structure for this format
    if (format == QStringLiteral("background"))
        structure = new BackgroundPackage();
    else if (format == QStringLiteral("element"))
        structure = new ElementPackage();
    else if (format == QStringLiteral("containment"))
        structure = new ContainmentPackage();
    else if (format == QStringLiteral("shell"))
        structure = new ShellPackage();
    else if (format == QStringLiteral("lookandfeel"))
        structure = new LookAndFeelPackage();

    // Create and return a package if there's a structure for the format
    if (structure) {
        d->structures.insert(format, structure);
        return Package(structure);
    }

    return Package();
}

} // namespace Shell

} // namespace Hawaii
