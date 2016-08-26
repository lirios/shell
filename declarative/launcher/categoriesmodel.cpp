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
    QString category;

    static bool lessThan(CategoryEntry *e1, CategoryEntry *e2) {
        return e1->name < e2->name;
    }
};

CategoriesModel::CategoriesModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_allCategory(true)
{
    refresh();
}

CategoriesModel::~CategoriesModel()
{
    qDeleteAll(m_list);
}

bool CategoriesModel::isAllCategoryIncluded() const
{
    return m_allCategory;
}

void CategoriesModel::includeAllCategory(bool value)
{
    if (m_allCategory == value)
        return;

    // Remove the item if it was previously included
    if (m_allCategory) {
        beginResetModel();
        delete m_list.takeAt(0);
        endResetModel();
    }

    m_allCategory = value;
    Q_EMIT includeAllCategoryChanged();
}

QHash<int, QByteArray> CategoriesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(NameRole, "name");
    roles.insert(CommentRole, "comment");
    roles.insert(IconNameRole, "iconName");
    roles.insert(CategoryRole, "category");
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
    case CategoryRole:
        return item->category;
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

    if (m_allCategory) {
        CategoryEntry *allCategory = new CategoryEntry();
        allCategory->name = tr("All");
        allCategory->comment = tr("All categories");
        allCategory->iconName = QStringLiteral("applications-other");
        allCategory->category = QString();
        m_list.append(allCategory);
    }

    XdgMenu xdgMenu;
    //xdgMenu.setLogDir(QStringLiteral("/tmp/"));
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Hawaii") << QStringLiteral("X-Hawaii"));
    const QString menuFileName = XdgMenu::getMenuFileName();
    if (!xdgMenu.read(menuFileName)) {
        qWarning("Failed to read menu \"%s\": %s", qPrintable(menuFileName),
                 qPrintable(xdgMenu.errorString()));
        return;
    }

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
            entry->category = xml.attribute(QStringLiteral("name"));
            m_list.append(entry);
        }
    }

    if (m_allCategory)
        qSort(m_list.begin() + 1, m_list.end(), CategoryEntry::lessThan);
    else
        qSort(m_list.begin(), m_list.end(), CategoryEntry::lessThan);

    endResetModel();
}

#include "moc_categoriesmodel.cpp"
