/****************************************************************************
 * This file is part of Hawaii.
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

/**
 * Provides a simple proxy model for accessing a subset, or "page," of data from a source model.
 * This is used by PagedGrid to provide models to each page with the appropriate subset of data.
 */
class PageModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int startIndex READ startIndex WRITE setStartIndex NOTIFY startIndexChanged)
    Q_PROPERTY(int limitCount READ limitCount WRITE setLimitCount NOTIFY limitCountChanged)

public:
    PageModel(QObject *parent = nullptr);

    int startIndex() const;
    int limitCount() const;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

public Q_SLOTS:
    void setStartIndex(int startIndex);
    void setLimitCount(int limitCount);

Q_SIGNALS:
    void startIndexChanged();
    void limitCountChanged();

private:
    int m_startIndex = 0;
    int m_limitCount = 0;
};

QML_DECLARE_TYPE(PageModel)
