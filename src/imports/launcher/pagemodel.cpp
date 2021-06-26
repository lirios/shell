// SPDX-FileCopyrightText: 2017 Michael Spencer <sonrisesoftware@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pagemodel.h"

#include <QtCore/QtGlobal>

PageModel::PageModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

bool PageModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    return sourceRow >= m_startIndex && sourceRow < m_startIndex + m_limitCount;
}

int PageModel::startIndex() const { return m_startIndex; }

int PageModel::limitCount() const { return m_limitCount; }

void PageModel::setStartIndex(int startIndex)
{
    if (startIndex != m_startIndex) {
        m_startIndex = startIndex;
        invalidateFilter();
        Q_EMIT startIndexChanged();
    }
}

void PageModel::setLimitCount(int limitCount)
{
    if (limitCount != m_limitCount) {
        m_limitCount = limitCount;
        invalidateFilter();
        Q_EMIT limitCountChanged();
    }
}
