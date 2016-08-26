/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

#include <Hawaii/Core/DesktopFile>
#include <QtGui/QIcon>

#include "application.h"
#include "usagetracker.h"

using namespace Hawaii;

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Settings
    m_settings = new QGSettings(QStringLiteral("org.hawaiios.desktop.panel"),
                                QStringLiteral("/org/hawaiios/desktop/panel/"), this);

    // Add pinned launchers
    const QStringList pinnedLaunchers =
        m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size() + pinnedLaunchers.size());
    Q_FOREACH (const QString &appId, pinnedLaunchers)
        m_list << new Application(appId, true, this);
    endInsertRows();
}

ApplicationManager *LauncherModel::applicationManager() const
{
    return m_appMan;
}

void LauncherModel::setApplicationManager(ApplicationManager *appMan)
{
    if (m_appMan == appMan)
        return;

    if (m_appMan) {
        disconnect(m_appMan, &ApplicationManager::applicationAdded, this,
                   &LauncherModel::handleApplicationAdded);
        disconnect(m_appMan, &ApplicationManager::applicationRemoved, this,
                   &LauncherModel::handleApplicationRemoved);
        disconnect(m_appMan, &ApplicationManager::applicationFocused, this,
                   &LauncherModel::handleApplicationFocused);
    }

    m_appMan = appMan;
    Q_EMIT applicationManagerChanged();

    if (appMan) {
        connect(appMan, &ApplicationManager::applicationAdded, this,
                &LauncherModel::handleApplicationAdded);
        connect(appMan, &ApplicationManager::applicationRemoved, this,
                &LauncherModel::handleApplicationRemoved);
        connect(appMan, &ApplicationManager::applicationFocused, this,
                &LauncherModel::handleApplicationFocused);
    }
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
    return m_list.size();
}

QVariant LauncherModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Application *app = m_list.at(index.row());

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
    if (index < 0 || index >= m_list.size())
        return nullptr;
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
    Application *found = nullptr;
    int pinAtIndex = 0;

    Q_FOREACH (Application *app, m_list) {
        if (app->isPinned())
            pinAtIndex++;

        if (app->appId() == appId) {
            found = app;
            break;
        }
    }

    if (!found)
        return;

    found->setPinned(true);

    int foundIndex = m_list.indexOf(found);

    if (foundIndex != pinAtIndex) {
        moveRows(foundIndex, 1, pinAtIndex);

        QModelIndex modelIndex = index(pinAtIndex);
        Q_EMIT dataChanged(modelIndex, modelIndex);
    } else {
        QModelIndex modelIndex = index(foundIndex);
        Q_EMIT dataChanged(modelIndex, modelIndex);
    }

    pinLauncher(appId, true);
}

void LauncherModel::unpin(const QString &appId)
{
    Application *found = nullptr;

    Q_FOREACH (Application *app, m_list) {
        if (!app->isPinned())
            break;

        if (app->appId() == appId) {
            found = app;
            break;
        }
    }

    if (!found)
        return;

    Q_ASSERT(found->isPinned());

    int i = m_list.indexOf(found);

    // Remove the item when unpinned and not running
    if (found->isRunning()) {
        found->setPinned(false);
        found->setPinned(false);
        moveRows(i, 1, m_list.size() - 1);

        QModelIndex modelIndex = index(m_list.size() - 1);
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
    QStringList pinnedLaunchers =
        m_settings->value(QStringLiteral("pinnedLaunchers")).toStringList();

    // Add or remove from the pinned launchers
    if (pinned)
        pinnedLaunchers.append(appId);
    else
        pinnedLaunchers.removeOne(appId);
    m_settings->setValue(QStringLiteral("pinnedLaunchers"), pinnedLaunchers);
}

void LauncherModel::handleApplicationAdded(QString appId, pid_t pid)
{
    appId = DesktopFile::canonicalAppId(appId);

    // Do we have already an icon?
    for (int i = 0; i < m_list.size(); i++) {
        Application *app = m_list.at(i);
        if (app->appId() == appId) {
            app->m_pids.insert(pid);
            app->setState(Application::Running);
            QModelIndex modelIndex = index(i);
            Q_EMIT dataChanged(modelIndex, modelIndex);
            return;
        }
    }

    // Otherwise create one
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    Application *app = new Application(appId, this);
    app->setState(Application::Running);
    app->m_pids.insert(pid);
    m_list.append(app);
    endInsertRows();

    if (!app->desktopFile()->noDisplay())
        UsageTracker::instance()->applicationLaunched(appId);
}

void LauncherModel::handleApplicationRemoved(QString appId, pid_t pid)
{
    appId = DesktopFile::canonicalAppId(appId);

    UsageTracker::instance()->applicationFocused(QString());

    for (int i = 0; i < m_list.size(); i++) {
        Application *app = m_list.at(i);
        if (app->appId() == appId) {
            // Remove this pid and determine if there are any processes left
            app->m_pids.remove(pid);
            if (app->m_pids.count() > 0)
                return;

            if (app->isPinned()) {
                // If it's pinned we just unset the flags if all pids are gone
                app->setState(Application::NotRunning);
                app->setActive(false);
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
}

void LauncherModel::handleApplicationFocused(QString appId)
{
    appId = DesktopFile::canonicalAppId(appId);

    Application *found = nullptr;

    for (int i = 0; i < m_list.size(); i++) {
        Application *app = m_list.at(i);
        bool changed = false;

        if (app->appId() == appId) {
            found = app;
            changed = !app->isActive();
            app->setActive(true);
        } else {
            changed = app->isActive();
            app->setActive(false);
        }

        if (changed) {
            QModelIndex modelIndex = index(i);
            Q_EMIT dataChanged(modelIndex, modelIndex);
        }
    }

    if (found != nullptr && !found->desktopFile()->noDisplay())
        UsageTracker::instance()->applicationFocused(appId);
    else
        UsageTracker::instance()->applicationFocused(QString());
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
            Q_ASSERT(m_list[i]);
            tmp << m_list.takeAt(i);
        }
        for (int i = 0; i < count; i++) {
            Q_ASSERT(tmp[i]);
            m_list.insert(destinationChild - count + 2 + i, tmp[i]);
        }
        endMoveRows();
    } else if (sourceRow > destinationChild) {
        beginMoveRows(QModelIndex(), sourceRow, sourceRow + count - 1, QModelIndex(),
                      destinationChild);
        tmp.reserve(count);
        for (int i = sourceRow; i < sourceRow + count; i++) {
            Q_ASSERT(m_list[i]);
            tmp << m_list.takeAt(i);
        }
        for (int i = 0; i < count; i++) {
            Q_ASSERT(tmp[i]);
            m_list.insert(destinationChild + i, tmp[i]);
        }
        endMoveRows();
    }
    return true;
}

#include "moc_launchermodel.cpp"
