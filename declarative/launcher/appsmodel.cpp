/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
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

#include <QtCore/QFileSystemWatcher>
#include <QtGui/QIcon>

#include "application.h"
#include "appsmodel.h"
#include "utils.h"

AppsModel::AppsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPaths(xdgApplicationsPaths());
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) {
#if 0
        // FIXME: This will crash V4 because refresh() will delete the applications
        if (m_appMan) {
            ApplicationManager *appMan = m_appMan;
            setApplicationManager(nullptr);

            beginResetModel();
            m_appMan->refresh();
            endResetModel();

            setApplicationManager(appMan);
        }
#endif
    });
}

ApplicationManager *AppsModel::applicationManager() const
{
    return m_appMan;
}

void AppsModel::setApplicationManager(ApplicationManager *appMan)
{
    if (m_appMan == appMan)
        return;

    m_appMan = appMan;

    if (appMan != nullptr) {
        beginResetModel();
        m_apps = m_appMan->applications();
        endResetModel();

        connect(m_appMan, &ApplicationManager::applicationAdded, this, &AppsModel::setupConnections);

        Q_FOREACH (Application *app, m_appMan->applications()) {
            setupConnections(app);
        }
    }

    Q_EMIT applicationManagerChanged();
}

void AppsModel::setupConnections(Application *app)
{
    connect(app, &Application::stateChanged, this, [this, app] {
        int i = m_apps.indexOf(app);

        if (i >= 0) {
            QModelIndex modelIndex = index(i);
            Q_EMIT dataChanged(modelIndex, modelIndex);
        }
    });
}

QHash<int, QByteArray> AppsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(NameRole, "name");
    roles.insert(GenericNameRole, "genericName");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    roles.insert(FilterInfoRole, "filterInfo");
    roles.insert(RunningRole, "running");
    roles.insert(CategoriesRole, "categories");
    roles.insert(AppIdRole, "appId");
    return roles;
}

int AppsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_apps.count();
}

QVariant AppsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    Application *app = m_apps[index.row()];

    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(app->iconName());
    case Qt::DisplayRole:
    case NameRole:
        return app->name();
    case GenericNameRole:
        return app->genericName();
    case CommentRole:
        return app->comment();
    case IconNameRole:
        return app->iconName();
    case AppIdRole:
        return app->appId();
    case FilterInfoRole:
        return QString(app->name() + app->comment());
    case RunningRole:
        return app->isRunning();
    case CategoriesRole:
        return app->categories();
    default:
        break;
    }

    return QVariant();
}
