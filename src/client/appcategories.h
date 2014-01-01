/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2014 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef APPCATEGORIES_H
#define APPCATEGORIES_H

#include <QtCore/QAbstractListModel>

class AppCategories : public QAbstractListModel
{
    Q_OBJECT
public:
    enum {
        NameRole = Qt::UserRole + 1,
        LabelRole
    };

    explicit AppCategories(QObject *parent = 0);

    QHash<int, QByteArray> roleNames() const;

    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;

private:
    typedef QPair<QString, QString> AppCategoryPair;
    QList<AppCategoryPair> m_categories;
};

#endif // APPCATEGORIES_H
