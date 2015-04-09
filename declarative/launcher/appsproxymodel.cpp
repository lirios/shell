/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include "appsmodel.h"
#include "appsproxymodel.h"

AppsProxyModel::AppsProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // Case insensitive filtering
    setFilterCaseSensitivity(Qt::CaseInsensitive);

    // Filter the right role
    setFilterRole(AppsModel::FilterInfoRole);

    // Source model
    AppsModel *model = new AppsModel(this);
    connect(model, &AppsModel::appLaunched, this, &AppsProxyModel::appLaunched);
    setSourceModel(model);
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

QModelIndex AppsProxyModel::sourceIndex(const QModelIndex &proxyIndex) const
{
    return mapToSource(proxyIndex);
}

bool AppsProxyModel::trigger(const QModelIndex &proxyIndex)
{
    return static_cast<AppsModel *>(sourceModel())->trigger(sourceIndex(proxyIndex));
}

#include "moc_appsproxymodel.cpp"
