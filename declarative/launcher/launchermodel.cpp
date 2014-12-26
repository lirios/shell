/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2014 Pier Luigi Fiorini
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

#include <QtGui/QIcon>

#include "launcheritem.h"
#include "launchermodel.h"

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size() + 2);
    m_list.append(new LauncherItem("chromium"));
    m_list.append(new LauncherItem("xchat"));
    endInsertRows();
}

LauncherModel::~LauncherModel()
{
    // Delete the items
    while (!m_list.empty())
        m_list.takeFirst()->deleteLater();
}

QHash<int, QByteArray> LauncherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(IconNameRole, "iconName");
    roles.insert(AppIdRole, "appId");
    roles.insert(PinnedRole, "pinned");
    roles.insert(RunningRole, "running");
    roles.insert(ActiveRole, "active");
    roles.insert(HasCountRole, "hasCount");
    roles.insert(CountRole, "count");
    roles.insert(HasProgressRole, "hasProgress");
    roles.insert(ProgressRole, "progress");
    roles.insert(HasActionListRole, "hasAction");
    return roles;
}

int LauncherModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_list.size();
}

QVariant LauncherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    LauncherItem *item = m_list.at(index.row());

    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(item->iconName());
    case Qt::DisplayRole:
    case NameRole:
        return item->name();
    case IconNameRole:
        return item->iconName();
    case AppIdRole:
        return item->appId();
    case PinnedRole:
        return item->isPinned();
    case RunningRole:
        return item->isRunning();
    case ActiveRole:
        return item->isActive();
    case HasCountRole:
        return item->count() > 0;
    case CountRole:
        return item->count();
    case HasProgressRole:
        return item->progress() >= 0;
    case ProgressRole:
        return item->progress();
    default:
        break;
    }

    return QVariant();
}

LauncherItem *LauncherModel::get(int index) const
{
    if (index < 0 || index >= m_list.size())
        return Q_NULLPTR;
    return m_list.at(index);
}

#include "moc_launchermodel.cpp"
