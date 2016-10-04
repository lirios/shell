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
