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

#include <QtCore/QDir>
#include <QtCore/QSet>
#include <QtCore/QStandardPaths>

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

Package PluginLoader::loadPackage(PluginType type)
{
    Q_D(PluginLoader);

    // Return a previously created package structure for this format
    PackageStructure *structure = d->structures.value(type).data();
    if (structure)
        return Package(structure);

    // Create a package structure for this format
    switch (type) {
    case BackgroundPlugin:
        structure = new BackgroundPackage();
        break;
    case ElementPlugin:
        structure = new ElementPackage();
        break;
    case ContainmentPlugin:
        structure = new ContainmentPackage();
        break;
    case ShellPlugin:
        structure = new ShellPackage();
        break;
    case LookAndFeelPlugin:
        structure = new LookAndFeelPackage();
        break;
    case PreferencesPlugin:
        structure = new PreferencesPackage();
        break;
    default:
        break;
    }

    // Create and return a package if there's a structure for the format
    if (structure) {
        d->structures.insert(type, structure);
        return Package(structure);
    }

    return Package();
}

QStringList PluginLoader::listPlugins(PluginType type)
{
    QSet<QString> packageIds;

    Package package = loadPackage(type);

    QStringList directories =
            QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                      package.defaultPackageRoot(),
                                      QStandardPaths::LocateDirectory);
    for (QString directory: directories) {
        QDir dir(directory);

        for (QString item: dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
            packageIds.insert(item);
    }

    return packageIds.toList();
}

} // namespace Shell

} // namespace Hawaii
