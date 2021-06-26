// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
