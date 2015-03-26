/****************************************************************************
 * This file is part of Hawaii Shell.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#include <qt5xdg/xdgmenu.h>
#include <qt5xdg/xmlhelper.h>

#include "categoriesmodel.h"

class CategoryEntry
{
public:
    CategoryEntry() {}

    QString name;
    QString comment;
    QString iconName;
};

CategoriesModel::CategoriesModel(QObject *parent)
    : QAbstractListModel(parent)
{
    refresh();
}

CategoriesModel::~CategoriesModel()
{
    qDeleteAll(m_list);
}

QHash<int, QByteArray> CategoriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    return roles;
}

int CategoriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

QVariant CategoriesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    CategoryEntry *item = m_list.at(index.row());

    switch (role) {
    case Qt::DecorationRole:
        return QIcon::fromTheme(item->iconName);
    case Qt::DisplayRole:
    case NameRole:
        return item->name;
    case CommentRole:
        return item->comment;
    case IconNameRole:
        return item->iconName;
    default:
        break;
    }

    return QVariant();
}

void CategoriesModel::refresh()
{
    beginResetModel();

    Q_EMIT refreshing();

    qDeleteAll(m_list);
    m_list.clear();

    XdgMenu xdgMenu;
    xdgMenu.setEnvironments(QStringLiteral("X-Hawaii"));
    if (!xdgMenu.read(XdgMenu::getMenuFileName()))
        return;

    QDomElement xml = xdgMenu.xml().documentElement();

    DomElementIterator it(xml, QString());
    while (it.hasNext()) {
        QDomElement xml = it.next();

        if (xml.tagName() == QStringLiteral("Menu")) {
            CategoryEntry *entry = new CategoryEntry();
            if (!xml.attribute(QStringLiteral("title")).isEmpty())
                entry->name = xml.attribute(QStringLiteral("title"));
            else
                entry->name = xml.attribute(QStringLiteral("name"));
            entry->comment = xml.attribute(QStringLiteral("comment"));
            entry->iconName = xml.attribute(QStringLiteral("icon"));
            m_list.append(entry);
        }
    }

    endResetModel();
}

#include "moc_categoriesmodel.cpp"
