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

#include "packagesmodelitem.h"

using namespace Hawaii::Shell;

/*
 * PackagesModelItemPrivate
 */

class PackagesModelItemPrivate
{
public:
    PackagesModelItemPrivate()
    {
    }

    Package package;
};

/*
 * PackagesModelItem
 */

PackagesModelItem::PackagesModelItem(Package package, QObject *parent)
    : QQmlPropertyMap(parent)
    , d_ptr(new PackagesModelItemPrivate())
{
    Q_D(PackagesModelItem);
    d->package = package;

    insert(QStringLiteral("identifier"), package.metadata().internalName());
    insert(QStringLiteral("name"), package.metadata().name());
    insert(QStringLiteral("description"), package.metadata().comment());
}

PackagesModelItem::~PackagesModelItem()
{
    delete d_ptr;
}

QUrl PackagesModelItem::filePath(const char *key, const QString &fileName) const
{
    Q_D(const PackagesModelItem);

    if (!d->package.isValid())
        return QUrl();

    return QUrl::fromLocalFile(d->package.filePath(key, fileName));
}

#include "moc_packagesmodelitem.cpp"
