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

#include "launchermodel.h"

#include <QtCore/QTimer>
#include <QtGui/QIcon>

#include <LiriCore/DesktopFile>

#include "application.h"
#include "appsmodel.h"
#include "usagetracker.h"

using namespace Liri;

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

ApplicationManager *LauncherModel::applicationManager() const
{
    return m_appMan;
}

void LauncherModel::setApplicationManager(ApplicationManager *appMan)
{
    if (m_appMan == appMan)
        return;

    m_appMan = appMan;
    Q_EMIT applicationManagerChanged();

    if (appMan != nullptr) {
        connect(appMan, &ApplicationManager::applicationAdded,
                this, &LauncherModel::setupConnections);

        Q_FOREACH (Application *app, appMan->applications()) {
            setupConnections(app);
        }

        m_apps = appMan->pinnedApps();
    }
}

void LauncherModel::setupConnections(Application *app)
{
    connect(app, &Application::stateChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            if (app->state() == Application::NotRunning && !app->isPinned()) {
                beginRemoveRows(QModelIndex(), i, i);
                m_apps.removeAt(i);
                endRemoveRows();
            } else {
                QModelIndex modelIndex = index(i);
                Q_EMIT dataChanged(modelIndex, modelIndex);
            }
        } else {
            if (app->state() != Application::NotRunning) {
                beginInsertRows(QModelIndex(), m_apps.size(), m_apps.size());
                m_apps.append(app);
                endInsertRows();
            }
        }
    });

    connect(app, &Application::activeChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            QModelIndex modelIndex = index(i);
            Q_EMIT dataChanged(modelIndex, modelIndex);
        }
    });

    connect(app, &Application::pinnedChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            QModelIndex modelIndex = index(i);
            Q_EMIT dataChanged(modelIndex, modelIndex);

            // App is in the launcher.
            if (app->isPinned()) {
                // If pinned, move to the pinned section,
                moveRows(i, 1, pinAtIndex());
            } else if (app->isRunning()) {
                // Otherwise, if running, move the end
                moveRows(i, 1, m_apps.size() - 1);
            } else {
                // Otherwise, remove
                beginRemoveRows(QModelIndex(), i, i);
                m_apps.removeAt(i);
                endRemoveRows();
            }
        } else {
            // App is not in the launcher
            // If pinned, add to the launcher
            if (app->isPinned()) {
                int i = pinAtIndex();
                beginInsertRows(QModelIndex(), i, i);
                m_apps.insert(i, app);
                endInsertRows();
            }
        }
    });
}

int LauncherModel::pinAtIndex() const
{
    int lastIndex = 0;

    Q_FOREACH (Application *app, m_apps) {
        if (!app->isPinned())
            return lastIndex;

        lastIndex++;
    }

    return lastIndex - 1;
}

QHash<int, QByteArray> LauncherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(AppIdRole, "appId");
    roles.insert(DesktopFileRole, "desktopFile");
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    roles.insert(PinnedRole, "pinned");
    roles.insert(RunningRole, "running");
    roles.insert(StartingRole, "starting");
    roles.insert(ActiveRole, "active");
    roles.insert(HasWindowsRole, "hasWindows");
    roles.insert(HasCountRole, "hasCount");
    roles.insert(CountRole, "count");
    roles.insert(HasProgressRole, "hasProgress");
    roles.insert(ProgressRole, "progress");
    return roles;
}

int LauncherModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_apps.size();
}

QVariant LauncherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Application *app = m_apps.at(index.row());

    switch (role) {
    case DesktopFileRole:
        return QVariant::fromValue(app->desktopFile());
    case NameRole:
        return app->desktopFile()->name();
    case IconNameRole:
        return app->desktopFile()->iconName();
    case AppIdRole:
        return app->appId();
    case PinnedRole:
        return app->isPinned();
    case RunningRole:
        return app->isRunning();
    case StartingRole:
        return app->isStarting();
    case ActiveRole:
        return app->isActive();
    case HasWindowsRole:
        return false;
    case HasCountRole:
        return app->count() > 0;
    case CountRole:
        return app->count();
    case HasProgressRole:
        return app->progress() >= 0;
    case ProgressRole:
        return app->progress();
    default:
        break;
    }

    return QVariant();
}

Application *LauncherModel::get(int index) const
{
    if (index < 0 || index >= m_apps.size())
        return nullptr;
    return m_apps.at(index);
}

int LauncherModel::indexFromAppId(const QString &appId) const
{
    for (int i = 0; i < m_apps.size(); i++) {
        if (m_apps.at(i)->appId() == appId)
            return i;
    }

    return -1;
}

bool LauncherModel::moveRows(int sourceRow, int count, int destinationChild)
{
    return moveRows(QModelIndex(), sourceRow, count, QModelIndex(), destinationChild);
}

bool LauncherModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                             const QModelIndex &destinationParent, int destinationChild)
{
    QList<Application *> tmp;

    Q_UNUSED(sourceParent);
    Q_UNUSED(destinationParent);

    if (sourceRow + count - 1 < destinationChild) {
        beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(),
                      destinationChild + 1);
        tmp.reserve(count);
        for (int i = sourceRow; i < sourceRow + count; i++) {
            Q_ASSERT(m_apps[i]);
            tmp << m_apps.takeAt(i);
        }
        for (int i = 0; i < count; i++) {
            Q_ASSERT(tmp[i]);
            m_apps.insert(destinationChild - count + 2 + i, tmp[i]);
        }
        endMoveRows();
    } else if (sourceRow > destinationChild) {
        beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(),
                      destinationChild);
        tmp.reserve(count);
        for (int i = sourceRow; i < sourceRow + count; i++) {
            Q_ASSERT(m_apps[i]);
            tmp << m_apps.takeAt(i);
        }
        for (int i = 0; i < count; i++) {
            Q_ASSERT(tmp[i]);
            m_apps.insert(destinationChild + i, tmp[i]);
        }
        endMoveRows();
    }
    return true;
}

#include "moc_launchermodel.cpp"
