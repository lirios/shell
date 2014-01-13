/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2011 Aaron Seigo <aseigo@kde.org>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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

#include "packagestructure.h"

namespace Hawaii {

namespace Shell {

PackageStructure::PackageStructure(QObject *parent, const QVariantList &args)
    : QObject(parent)
    , d_ptr(nullptr)
{
    Q_UNUSED(args);
}

PackageStructure::~PackageStructure()
{
}

/*!
    Called when a the PackageStructure should initialize a Package with the initial
    structure. This allows setting paths before setPath is called.

    Note: one special value is "metadata" which can be set to the location of KPluginInfo
    compatible .desktop file within the package. If not defined, it is assumed that this file
    exists under the top level directory of the package.

    \param package the Package to set up. The object is empty of all definition when
                   first passed in.
*/
void PackageStructure::initializePackage(Package *package)
{
    Q_UNUSED(package);
}

/*!
    Called whenever the path changes so that subclasses may take
    package specific actions.
*/
void PackageStructure::pathChanged(Package *package)
{
    Q_UNUSED(package);
}

} // namespace Shell

} // namespace Hawaii

#include "moc_packagestructure.cpp"
