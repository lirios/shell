/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini
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

#include "applicationinfo.h"
#include "applicationmanager.h"
#include "launcheritem.h"
#include "launchermodel.h"

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_appMan(new ApplicationManager(this))
{
    // Connect to application events
    connect(m_appMan, &ApplicationManager::registered, this, [this](const QString &appId) {
        // Do we have already an icon?
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                item->setRunning(true);
                QModelIndex modelIndex = index(i);
                Q_EMIT dataChanged(modelIndex, modelIndex);
                return;
            }
        }

        // Otherwise create one
        beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
        m_list.append(new LauncherItem(appId, this));
        endInsertRows();
    });
    connect(m_appMan, &ApplicationManager::unregistered, this, [this](const QString &appId) {
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                if (item->isPinned()) {
                    // If it's pinned we just unset the flags
                    item->setRunning(false);
                    item->setActive(false);
                } else {
                    // Otherwise the icon goes away because it wasn't meant
                    // to stay
                    beginRemoveRows(QModelIndex(), i, i);
                    m_list.takeAt(i)->deleteLater();
                    endRemoveRows();
                }
                break;
            }
        }
    });
    connect(m_appMan, &ApplicationManager::focused, this, [this](const QString &appId) {
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                item->setActive(true);
                QModelIndex modelIndex = index(i);
                Q_EMIT dataChanged(modelIndex, modelIndex);
                break;
            }
        }
    });
    connect(m_appMan, &ApplicationManager::unfocused, this, [this](const QString &appId) {
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                item->setActive(false);
                QModelIndex modelIndex = index(i);
                Q_EMIT dataChanged(modelIndex, modelIndex);
                break;
            }
        }
    });

    // Add static items
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size() + 2);
    m_list.append(new LauncherItem("chromium", true, this));
    m_list.append(new LauncherItem("xchat", true, this));
    m_list.append(new LauncherItem("org.kde.konsole", true, this));
    m_list.append(new LauncherItem("weston-terminal", true, this));
    endInsertRows();
}

LauncherModel::~LauncherModel()
{
    // Delete the items
    while (!m_list.isEmpty())
        m_list.takeFirst()->deleteLater();
}

QHash<int, QByteArray> LauncherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(AppIdRole, "appId");
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    roles.insert(PinnedRole, "pinned");
    roles.insert(RunningRole, "running");
    roles.insert(ActiveRole, "active");
    roles.insert(HasWindowsRole, "hasWindows");
    roles.insert(HasCountRole, "hasCount");
    roles.insert(CountRole, "count");
    roles.insert(HasProgressRole, "hasProgress");
    roles.insert(ProgressRole, "progress");
    roles.insert(HasActionsRole, "hasActions");
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
    case HasWindowsRole:
        return false;
    case HasCountRole:
        return item->count() > 0;
    case CountRole:
        return item->count();
    case HasProgressRole:
        return item->progress() >= 0;
    case ProgressRole:
        return item->progress();
        /*
    case HasActionListRole:
        return item->actionsCount();
    case ActionListRole:
        return item->actions();
        */
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

int LauncherModel::indexFromAppId(const QString &appId) const
{
    for (int i = 0; i < m_list.size(); i++) {
        if (m_list.at(i)->appId() == appId)
            return 0;
    }

    return -1;
}

#include "moc_launchermodel.cpp"
