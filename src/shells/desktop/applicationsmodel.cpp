/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QStandardPaths>
#include <QtGui/QIcon>

#include "applicationsmodel.h"
#include "appinfo.h"

/*
 * ApplicationsModel
 */

ApplicationsModel::ApplicationsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Watch for changes in application directories
    m_watcher = new QFileSystemWatcher(this);
    m_watcher->addPaths(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation));
    connect(m_watcher, SIGNAL(directoryChanged(QString)),
            this, SLOT(directoryChanged(QString)));

    // Populate the list
    populate();
}

QHash<int, QByteArray> ApplicationsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[NameRole] = "name";
    names[CommentRole] = "comment";
    names[IconNameRole] = "iconName";
    names[CategoriesRole] = "categories";
    return names;
}

QVariant ApplicationsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    AppInfo *info = m_apps.at(index.row());

    switch (role) {
    case NameRole:
        return info->name();
    case CommentRole:
        return info->comment();
    case IconNameRole:
        return info->iconName();
    case CategoriesRole:
        return info->categories();
    default:
        break;
    }

    return QVariant();
}

int ApplicationsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_apps.size();
}

AppInfo *ApplicationsModel::get(int index)
{
    if (index < 0 || index > m_apps.size())
        return 0;
    return m_apps.at(index);
}

void ApplicationsModel::populate()
{
    QStringList paths = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
    for (int i = 0; i < paths.size(); i++)
        directoryChanged(paths.at(i));

    cleanupCategories();
}

void ApplicationsModel::cleanupCategories()
{
    // Remove other desktop's categories
    m_categories.remove("Qt");
    m_categories.remove("KDE");
    m_categories.remove("GNOME");
}

void ApplicationsModel::directoryChanged(const QString &path)
{
    // Remove items from this path
    for (int i = 0; i < m_apps.size(); i++) {
        AppInfo *info = m_apps.at(i);

        if (info->fileName().startsWith(path)) {
            beginRemoveRows(QModelIndex(), i, i);
            m_apps.removeAt(i);
            delete info;
            endRemoveRows();
        }
    }

    QDirIterator walker(path,
                        QDir::Files | QDir::NoDotAndDotDot | QDir::Readable,
                        QDirIterator::Subdirectories);
    while (walker.hasNext()) {
        walker.next();

        if (walker.fileInfo().completeSuffix() == "desktop") {
            QString fullPath = walker.fileInfo().absoluteFilePath();

            // Add this item (only if it can be displayed)
            AppInfo *info = new AppInfo();
            if (info->load(fullPath)) {
                if (info->isHidden() && info->isExecutable()) {
                    // Append item to the model
                    beginInsertRows(QModelIndex(), m_apps.size(), m_apps.size());
                    m_apps.append(info);
                    endInsertRows();

                    // Save categories
                    m_categories += QSet<QString>::fromList(info->categories());
                }
            }
        }
    }
}

/*
 * ApplicationsModelSorted
 */

ApplicationsModelSorted::ApplicationsModelSorted(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    m_model = new ApplicationsModel();
    setSourceModel(m_model);
    setSortRole(ApplicationsModel::NameRole);
    setSortLocaleAware(true);
    setSortCaseSensitivity(Qt::CaseSensitive);
    setDynamicSortFilter(true);
}

ApplicationsModelSorted::~ApplicationsModelSorted()
{
    delete m_model;
}

#include "moc_applicationsmodel.cpp"
