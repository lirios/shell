/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include "appsmodel.h"
#include "appsproxymodel.h"

AppsProxyModel::AppsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_sourceModel(Q_NULLPTR)
{
    // Case insensitive filtering
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

QString AppsProxyModel::query() const
{
    return m_query;
}

void AppsProxyModel::setQuery(const QString &query)
{
    if (m_query == query)
        return;

    setFilterRegExp(query);
    m_query = query;
    Q_EMIT queryChanged();
}

AppsModel *AppsProxyModel::model() const
{
    return m_sourceModel;
}

void AppsProxyModel::setModel(AppsModel *model)
{
    if (m_sourceModel == model)
        return;

    m_sourceModel = model;
    setSourceModel(model);
    setFilterRole(AppsModel::FilterInfoRole);
    Q_EMIT modelChanged();
}

QModelIndex AppsProxyModel::sourceIndex(const QModelIndex &proxyIndex) const
{
    return mapToSource(proxyIndex);
}

#include "moc_appsproxymodel.cpp"
