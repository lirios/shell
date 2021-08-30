// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "application.h"
#include "applicationsmodel.h"
#include "tasksmodel.h"

TasksModel::TasksModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    setSortLocaleAware(true);
    sort(0);
}

void TasksModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_appsModel = qobject_cast<ApplicationsModel *>(sourceModel);
    if (!m_appsModel) {
        qCWarning(lcTaskManager, "Invalid source model for TasksModel, must be an ApplicationsModel");
        return;
    }

    connect(m_appsModel, &ApplicationsModel::hasMaximizedWindowChanged,
            this, &TasksModel::hasMaximizedWindowChanged);
    connect(m_appsModel, &ApplicationsModel::refreshed,
            this, &TasksModel::invalidate);

    QSortFilterProxyModel::setSourceModel(m_appsModel);
}

bool TasksModel::hasMaximizedWindow() const
{
    return m_appsModel->hasMaximizedWindow();
}

Application *TasksModel::get(int row) const
{
    QModelIndex sourceIndex = mapToSource(index(row, 0));
    return m_appsModel->get(sourceIndex.row());
}

int TasksModel::indexFromAppId(const QString &appId) const
{
    int sourceRow = m_appsModel->indexFromAppId(appId);
    QModelIndex index = mapFromSource(m_appsModel->index(sourceRow));
    return index.row();
}

bool TasksModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    const QModelIndex &sourceIndex = sourceModel()->index(sourceRow, 0);

    if (sourceIndex.data(ApplicationsModel::PinnedRole).toBool())
        return true;
    if (sourceIndex.data(ApplicationsModel::RunningRole).toBool())
        return true;

    return false;
}

bool TasksModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
    bool leftPinned = sourceModel()->data(sourceLeft, ApplicationsModel::PinnedRole).toBool();
    int leftPinnedIndex = sourceModel()->data(sourceLeft, ApplicationsModel::PinnedIndexRole).toInt();
    QString leftName = sourceModel()->data(sourceLeft, ApplicationsModel::NameRole).toString();

    bool rightPinned = sourceModel()->data(sourceRight, ApplicationsModel::PinnedRole).toBool();
    int rightPinnedIndex = sourceModel()->data(sourceRight, ApplicationsModel::PinnedIndexRole).toInt();
    QString rightName = sourceModel()->data(sourceRight, ApplicationsModel::NameRole).toString();

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
