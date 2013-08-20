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

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QStandardPaths>
#include <QtCore/QStringBuilder>

#include "launchermodel.h"
#include "launcheritem.h"
#include "launchersettings.h"

LauncherModel::LauncherModel(QObject *parent)
    : QAbstractListModel(parent)
{
    // Settings
    m_settings = new LauncherSettings(this);

    // Load pinned applications
    foreach(QString fileName, m_settings->favoriteApps()) {
        QString fileNameFound = QStandardPaths::locate(QStandardPaths::ApplicationsLocation, fileName);
        QFileInfo fileInfo(fileNameFound);
        if (fileInfo.exists())
            pinApplication(fileInfo.filePath());
    }
}

LauncherModel::~LauncherModel()
{
    delete m_settings;
}

QHash<int, QByteArray> LauncherModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IsApplicationRole] = "isApplication";
    roles[IsUrlRole] = "isUrl";
    roles[LabelRole] = "label";
    roles[IconNameRole] = "iconName";
    roles[UrlRole] = "url";
    roles[IsActiveRole] = "active";
    roles[IsRunningRole] = "running";
    roles[IsUrgenteRole] = "urgent";
    roles[IsDraggableRole] = "draggable";
    roles[IsEditableRole] = "editable";
    roles[IsRemovableRole] = "removable";
    roles[IsCounterVisibleRole] = "counterVisible";
    roles[CounterRole] = "counter";
    return roles;
}

QVariant LauncherModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid())
        return QVariant();

    LauncherItem *item = m_list.at(index.row());

    switch (role) {
    case IsApplicationRole:
        return item->type() == LauncherItem::ApplicationItem;
    case IsUrlRole:
        return item->type() == LauncherItem::UrlItem;
    case LabelRole:
        return item->label();
    case IconNameRole:
        return item->iconName();
    case UrlRole:
        return item->url();
    case IsActiveRole:
        return item->isActive();
    case IsRunningRole:
        return item->isRunning();
    case IsUrgenteRole:
        return item->isUrgent();
    case IsDraggableRole:
        return item->isDraggable();
    case IsEditableRole:
        return item->isEditable();
    case IsRemovableRole:
        return item->isRemovable();
    case IsCounterVisibleRole:
        return item->isCounterVisible();
    case CounterRole:
        return item->counter();
    case Qt::DisplayRole:
        return item->label();
    default:
        break;
    }

    return QVariant();
}

int LauncherModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_list.size();
}

LauncherItem *LauncherModel::get(int index)
{
    if (index < 0 || index > m_list.size())
        return 0;
    return m_list.at(index);
}

void LauncherModel::pinApplication(const QString &fileName)
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(new LauncherItem(fileName));
    endInsertRows();
}

void LauncherModel::pinUrl(const QUrl &url)
{
    beginInsertRows(QModelIndex(), m_list.size(), m_list.size());
    m_list.append(new LauncherItem(url));
    endInsertRows();
}

#include "moc_launchermodel.cpp"
