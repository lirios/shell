/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL3+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef CATEGORIESMODEL_H
#define CATEGORIESMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

class CategoryEntry;

class CategoriesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool includeAllCategory READ isAllCategoryIncluded WRITE includeAllCategory NOTIFY includeAllCategoryChanged)
public:
    enum Roles {
        NameRole = Qt::UserRole +1,
        CommentRole,
        IconNameRole,
        CategoryRole
    };
    Q_ENUM(Roles)

    CategoriesModel(QObject *parent = 0);
    ~CategoriesModel();

    bool isAllCategoryIncluded() const;
    void includeAllCategory(bool value);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

Q_SIGNALS:
    void includeAllCategoryChanged();
    void refreshing();

private:
    QList<CategoryEntry *> m_list;
    bool m_allCategory;

    void refresh();
};

QML_DECLARE_TYPE(CategoriesModel)

#endif // CATEGORIESMODEL_H
