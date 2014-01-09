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

#include <QtCore/QList>
#include <QtCore/QPair>

#include "appcategories.h"

namespace Hawaii {

namespace Shell {

AppCategories::AppCategories(QObject *parent)
    : QAbstractListModel(parent)
{
    m_categories.append(AppCategoryPair("AudioVideo", tr("Sound & Video")));
    m_categories.append(AppCategoryPair("Audio", tr("Sound")));
    m_categories.append(AppCategoryPair("Video", tr("Video")));
    m_categories.append(AppCategoryPair("Development", tr("Programming")));
    m_categories.append(AppCategoryPair("Education", tr("Education")));
    m_categories.append(AppCategoryPair("Game", tr("Game")));
    m_categories.append(AppCategoryPair("Graphics", tr("Graphics")));
    m_categories.append(AppCategoryPair("Network", tr("Network")));
    m_categories.append(AppCategoryPair("Office", tr("Office")));
    m_categories.append(AppCategoryPair("Science", tr("Science")));
    m_categories.append(AppCategoryPair("Settings", tr("Settings")));
    m_categories.append(AppCategoryPair("System", tr("System")));
    m_categories.append(AppCategoryPair("Utility", tr("Utilities")));
}


QHash<int, QByteArray> AppCategories::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[LabelRole] = "label";
    return roles;
}

QVariant AppCategories::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
        case LabelRole:
        case Qt::DisplayRole:
            return m_categories.at(index.row()).second;
        case NameRole:
            return m_categories.at(index.row()).first;
        default:
            break;
    }

    return QVariant();
}

int AppCategories::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_categories.size();
}

} // namespace Shell

} // namespace Hawaii

#include "moc_appcategories.cpp"
