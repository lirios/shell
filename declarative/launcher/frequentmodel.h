/****************************************************************************
 * This file is part of Liri.
 *
 * Copyright (C) 2016 Michael Spencer <sonrisesoftware@gmail.com>
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

#pragma once

#include <QtCore/QSortFilterProxyModel>
#include <QtQml/QQmlComponent>

class FrequentAppsModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    FrequentAppsModel(QObject *parent = nullptr);

    int startIndex() const;
    int limitCount() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

QML_DECLARE_TYPE(FrequentAppsModel)
