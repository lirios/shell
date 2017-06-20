/****************************************************************************
 * This file is part of Liri.
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

#include <QtGui/QIcon>
#include <QDebug>

#include "applicationmanager.h"
#include "launchermodel.h"

LauncherModel::LauncherModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_appMan(nullptr)
{
    setSortLocaleAware(true);
    sort(0);
}

void LauncherModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_appMan = dynamic_cast<ApplicationManager *>(sourceModel);
    if (m_appMan)
        connect(m_appMan, &ApplicationManager::refreshed, this, &LauncherModel::invalidate);

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

Application *LauncherModel::get(int row) const
{
    if (!m_appMan)
        return nullptr;

    QModelIndex sourceIndex = mapToSource(index(row, 0));
    return m_appMan->get(sourceIndex.row());
}

int LauncherModel::indexFromAppId(const QString &appId) const
{
    if (!m_appMan)
        return -1;

    int sourceRow = m_appMan->indexFromAppId(appId);
    QModelIndex index = mapFromSource(m_appMan->index(sourceRow));
    return index.row();
}

bool LauncherModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    const QModelIndex &sourceIndex = sourceModel()->index(sourceRow, 0);

    if (sourceIndex.data(ApplicationManager::PinnedRole).toBool())
        return true;
    if (sourceIndex.data(ApplicationManager::RunningRole).toBool())
        return true;

    return false;
}

bool LauncherModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    bool leftPinned = sourceModel()->data(sourceLeft, ApplicationManager::PinnedRole).toBool();
    int leftPinnedIndex = sourceModel()->data(sourceLeft, ApplicationManager::PinnedIndexRole).toInt();
    QString leftName = sourceModel()->data(sourceLeft, ApplicationManager::NameRole).toString();

    bool rightPinned = sourceModel()->data(sourceRight, ApplicationManager::PinnedRole).toBool();
    int rightPinnedIndex = sourceModel()->data(sourceRight, ApplicationManager::PinnedIndexRole).toInt();
    QString rightName = sourceModel()->data(sourceRight, ApplicationManager::NameRole).toString();

    // If both are pinned we want to decided based on the index
    if (leftPinned && rightPinned) {
        if (leftPinnedIndex == -1)
            return false;
        return leftPinnedIndex < rightPinnedIndex;
    }

    // Decide based on who is pinned
    if (leftPinned && !rightPinned)
        return true;
    else if (!leftPinned && rightPinned)
        return false;

    // Both are not pinned, decided based on the name
    return leftName < rightName;
}

#include "moc_launchermodel.cpp"
