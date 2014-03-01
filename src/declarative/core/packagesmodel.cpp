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
#include <QtCore/QStandardPaths>

#include <HawaiiShell/Package>
#include <HawaiiShell/PluginLoader>

#include "packagesmodel.h"
#include "packagesmodelitem.h"

using namespace Hawaii::Shell;

/*
 * PackagesModelPrivate
 */

class PackagesModelPrivate
{
public:
    PackagesModelPrivate(PackagesModel *self);

    void populate();
    void clean();

    PackagesModel::Type type;
    QStringList identifiers;
    QList<Package> packages;

protected:
    Q_DECLARE_PUBLIC(PackagesModel)
    PackagesModel *q_ptr;
};

PackagesModelPrivate::PackagesModelPrivate(PackagesModel *self)
    : type(PackagesModel::InvalidPackage)
    , q_ptr(self)
{
}

void PackagesModelPrivate::populate()
{
    Q_Q(PackagesModel);

    PluginLoader::PluginType pluginType;
    switch (type) {
    case PackagesModel::BackgroundPackage:
        pluginType = PluginLoader::BackgroundPlugin;
        break;
    case PackagesModel::ElementPackage:
        pluginType = PluginLoader::ElementPlugin;
        break;
    case PackagesModel::ContainmentPackage:
        pluginType = PluginLoader::ContainmentPlugin;
        break;
    case PackagesModel::ShellPackage:
        pluginType = PluginLoader::ShellPlugin;
        break;
    case PackagesModel::LookAndFeelPackage:
        pluginType = PluginLoader::LookAndFeelPlugin;
        break;
    default:
        return;
    }

    Package package = PluginLoader::instance()->loadPackage(pluginType);

    QStringList pkgs(QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
                                               package.defaultPackageRoot(),
                                               QStandardPaths::LocateDirectory));
    for (QString path: pkgs) {
        QDir dir(path);

        for (QString pkgPath: dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Readable, QDir::Name)) {
            package.setPath(pkgPath);
            if (package.isValid()) {
                q->beginInsertRows(QModelIndex(), packages.size(), packages.size() + 1);
                packages.append(package);
                q->endInsertRows();
            }
        }
    }
}

void PackagesModelPrivate::clean()
{
    Q_Q(PackagesModel);

    if (packages.size() == 0)
        return;

    q->beginRemoveRows(QModelIndex(), 0, packages.size() - 1);
    packages.clear();
    q->endRemoveRows();
}

/*
 * PackagesModel
 */

PackagesModel::PackagesModel(QObject *parent)
    : QAbstractListModel(parent)
    , d_ptr(new PackagesModelPrivate(this))
{
}

PackagesModel::~PackagesModel()
{
    delete d_ptr;
}

PackagesModel::Type PackagesModel::type() const
{
    Q_D(const PackagesModel);
    return d->type;
}

void PackagesModel::setType(Type type)
{
    Q_D(PackagesModel);

    if (d->type != type) {
        d->type = type;
        d->clean();
        d->populate();
        Q_EMIT typeChanged();
    }
}

QHash<int, QByteArray> PackagesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdentifierRole] = "identifier";
    roles[NameRole] = "name";
    roles[DescriptionRole] = "description";
    roles[IconNameRole] = "iconName";
    return roles;
}

int PackagesModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const PackagesModel);
    Q_UNUSED(parent);

    return d->packages.size();
}

int PackagesModel::count() const
{
    return rowCount(QModelIndex());
}

QVariant PackagesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const PackagesModel);

    if (!index.isValid())
        return QVariant();

    Package package = d->packages.at(index.row());
    if (!package.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
    case IdentifierRole:
        return package.metadata().internalName();
    case NameRole:
        return package.metadata().name();
    case DescriptionRole:
        return package.metadata().comment();
    default:
        break;
    }

    return QVariant();
}

PackagesModelItem *PackagesModel::get(int index)
{
    Q_D(const PackagesModel);

    if (index < 0 || index >= d->packages.size())
        return nullptr;

    Package package = d->packages.at(index);
    if (!package.isValid())
        return nullptr;

    PackagesModelItem *item = new PackagesModelItem(package, this);
    return item;
}

#include "moc_packagesmodel.cpp"
