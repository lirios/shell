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

#ifndef PACKAGESMODEL_H
#define PACKAGESMODEL_H

#include <QtCore/QAbstractListModel>

class PackagesModelItem;
class PackagesModelPrivate;

class PackagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(int count READ count FINAL)
    Q_ENUMS(Type Roles)
public:
    enum Type {
        InvalidPackage = 0,
        BackgroundPackage,
        ElementPackage,
        ContainmentPackage,
        ShellPackage,
        LookAndFeelPackage
    };

    enum Roles {
        IdentifierRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        IconNameRole
    };

    explicit PackagesModel(QObject *parent = 0);
    ~PackagesModel();

    Type type() const;
    void setType(Type type);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent) const;

    Q_INVOKABLE int count() const;

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE PackagesModelItem *get(int index);

Q_SIGNALS:
    void typeChanged();

private:
    Q_DECLARE_PRIVATE(PackagesModel)
    PackagesModelPrivate *const d_ptr;
};

#endif // PACKAGESMODEL_H
