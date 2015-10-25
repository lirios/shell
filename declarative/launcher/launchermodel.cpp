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
#include <QDebug>

#include <GreenIsland/Server/ApplicationManager>
#include <Hawaii/QGSettings>

#include "applicationinfo.h"
#include "launcheritem.h"
#include "launchermodel.h"

using namespace GreenIsland;
using namespace Hawaii;

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Settings
    m_settings = new QGSettings(QStringLiteral("org.hawaiios.desktop.panel"),
                                QStringLiteral("/org/hawaiios/desktop/panel/"),
                                this);

    // Application manager instance
    ApplicationManager *appMan = ApplicationManager::instance();

    // Connect to application events
    connect(appMan, &ApplicationManager::applicationAdded, this, [this](const QString &appId, pid_t pid) {
        // Do we have already an icon?
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                item->m_pids.insert(pid);
                item->setRunning(true);
                QModelIndex modelIndex = index(i);
                Q_EMIT dataChanged(modelIndex, modelIndex);
                return;
            }
        }

        // Otherwise create one
        beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
        LauncherItem *item = new LauncherItem(appId, this);
        item->m_pids.insert(pid);
        m_list.append(item);
        endInsertRows();
    });
    connect(appMan, &ApplicationManager::applicationRemoved, this, [this](const QString &appId, pid_t pid) {
        for (int i = 0; i < m_list.size(); i++) {
            LauncherItem *item = m_list.at(i);
            if (item->appId() == appId) {
                // Remove this pid and determine if there are any processes left
                item->m_pids.remove(pid);
                if (item->m_pids.count() > 0)
                    return;

                if (item->isPinned()) {
                    // If it's pinned we just unset the flags if all pids are gone
                    item->setRunning(false);
                    item->setActive(false);
                    QModelIndex modelIndex = index(i);
                    Q_EMIT dataChanged(modelIndex, modelIndex);
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
    connect(appMan, &ApplicationManager::applicationFocused, this, [this](const QString &appId) {
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
    connect(appMan, &ApplicationManager::applicationUnfocused, this, [this](const QString &appId) {
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

    // Add pinned launchers
    const QStringList pinnedLaunchers = m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size() + pinnedLaunchers.size());
    Q_FOREACH (const QString &appId, pinnedLaunchers)
        m_list.append(new LauncherItem(appId, true, this));
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

void LauncherModel::pin(const QString &appId)
{
    LauncherItem *found = Q_NULLPTR;

    Q_FOREACH (LauncherItem *item, m_list) {
        if (item->appId() != appId)
            continue;

        found = item;
        break;
    }

    qDebug() << found;
    if (!found)
        return;

    found->setPinned(true);
    QModelIndex modelIndex = index(m_list.indexOf(found));
    Q_EMIT dataChanged(modelIndex, modelIndex);

    pinLauncher(appId, true);
}

void LauncherModel::unpin(const QString &appId)
{
    LauncherItem *found = Q_NULLPTR;

    Q_FOREACH (LauncherItem *item, m_list) {
        if (item->appId() != appId)
            continue;

        found = item;
        break;
    }

    if (!found)
        return;

    int i = m_list.indexOf(found);

    // Remove the item when unpinned and not running
    if (found->isRunning()) {
        found->setPinned(false);
        QModelIndex modelIndex = index(i);
        Q_EMIT dataChanged(modelIndex, modelIndex);
    } else {
        beginRemoveRows(QModelIndex(), i, i);
        m_list.takeAt(i)->deleteLater();
        endRemoveRows();
    }

    pinLauncher(appId, false);
}

void LauncherModel::pinLauncher(const QString &appId, bool pinned)
{
    // Currently pinned launchers
    QStringList pinnedLaunchers = m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();

    // Add or remove from the pinned launchers
    if (pinned)
        pinnedLaunchers.append(appId);
    else
        pinnedLaunchers.removeOne(appId);
    m_settings->setValue(QStringLiteral("pinnedLaunchers"), pinnedLaunchers);
}

#include "moc_launchermodel.cpp"
