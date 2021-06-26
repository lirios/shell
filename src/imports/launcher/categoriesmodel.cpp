// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QtCore/QFileSystemWatcher>
#include <QtCore/QFuture>
#include <QtCore/QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtGui/QIcon>

#include <LiriXdg/DesktopMenu>

#include "categoriesmodel.h"
#include "utils.h"

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
    , m_initialized(false)
    , m_allCategory(true)
{
    qRegisterMetaType<CategoryEntry *>("CategoryEntry*");

    QtConcurrent::run(CategoriesModel::refresh, this);

    QFileSystemWatcher *watcher = new QFileSystemWatcher(this);
    watcher->addPaths(xdgApplicationsPaths());
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) {
        QtConcurrent::run(CategoriesModel::refresh, this);
    });
}

CategoriesModel::~CategoriesModel()
{
    while (!m_list.isEmpty())
        delete m_list.takeFirst();
}

bool CategoriesModel::isAllCategoryIncluded() const
{
    return m_allCategory;
}

void CategoriesModel::includeAllCategory(bool value)
{
    if (m_allCategory == value)
        return;

    // Add or remove the item
    if (m_allCategory) {
        beginRemoveRows(QModelIndex(), 0, 0);
        delete m_list.takeAt(0);
        endRemoveRows();
    } else {
        createAllCategory();
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
    if (!item)
        return QVariant();

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

void CategoriesModel::refresh(CategoriesModel *model)
{
    // Create the 'All' category for the first time, if needed
    if (!model->m_initialized) {
        model->m_initialized = true;
        if (model->m_allCategory)
            QMetaObject::invokeMethod(model, "createAllCategory");
    }

    // List of categories that are no longer in the menu, at first
    // we assume that all of theme will have to be removed
    QStringList toRemove;
    for (CategoryEntry *category : qAsConst(model->m_list)) {
        if (category->name != tr("All"))
            toRemove.append(category->name);
    }

    QMetaObject::invokeMethod(model, "refreshing");

    Liri::DesktopMenu xdgMenu;
    //xdgMenu.setLogDir(QDir::tempPath()));
    xdgMenu.setEnvironments(QStringList() << QStringLiteral("Liri") << QStringLiteral("X-Liri"));
    const QString menuFileName = Liri::DesktopMenu::getMenuFileName();
    if (!xdgMenu.read(menuFileName)) {
        qWarning("Failed to read menu \"%s\": %s", qPrintable(menuFileName),
                 qPrintable(xdgMenu.errorString()));
        return;
    }

    QDomElement doc = xdgMenu.xml().documentElement();
    QDomElement xml = doc.firstChildElement();

    for (; !xml.isNull(); xml = xml.nextSiblingElement()) {
        if (xml.tagName() == QStringLiteral("Menu")) {
            QString name = xml.attribute(QStringLiteral("name"));
            if (!xml.attribute(QStringLiteral("title")).isEmpty())
                name = xml.attribute(QStringLiteral("title"));

            // This entry is still in the menu, we don't have to remove it
            toRemove.removeOne(name);

            // Do not add duplicate entries
            bool found = false;
            for (CategoryEntry *entry : qAsConst(model->m_list)) {
                if (entry->name == name) {
                    found = true;
                    break;
                }
            }
            if (found)
                break;

            // Create entry
            CategoryEntry *entry = new CategoryEntry();
            if (!xml.attribute(QStringLiteral("title")).isEmpty())
                entry->name = xml.attribute(QStringLiteral("title"));
            else
                entry->name = xml.attribute(QStringLiteral("name"));
            entry->comment = xml.attribute(QStringLiteral("comment"));
            entry->iconName = xml.attribute(QStringLiteral("icon"));
            entry->category = xml.attribute(QStringLiteral("name"));

            // Add entry
            QMetaObject::invokeMethod(model, "addEntry", Q_ARG(CategoryEntry*, entry));
        }
    }

    // Remove categories that are no longer in the menu
    //QMetaObject::invokeMethod(model, "cleanModel", Q_ARG(QStringList, toRemove));

    // Sort
    QMetaObject::invokeMethod(model, "sortModel");
}

void CategoriesModel::createAllCategory()
{
    beginInsertRows(QModelIndex(), 0, 0);
    CategoryEntry *allCategory = new CategoryEntry();
    allCategory->name = tr("All");
    allCategory->comment = tr("All categories");
    allCategory->iconName = QStringLiteral("applications-other");
    allCategory->category = QString();
    m_list.append(allCategory);
    endInsertRows();
}

void CategoriesModel::addEntry(CategoryEntry *entry)
{
    beginInsertRows(QModelIndex(), m_list.count(), m_list.count());
    m_list.append(entry);
    endInsertRows();
}

void CategoriesModel::cleanModel(const QStringList &list)
{
    for (const QString &name : qAsConst(list)) {
        auto it = m_list.begin();
        while (it != m_list.end()) {
            CategoryEntry *entry = (*it);
            if (entry->name == name) {
                int index = m_list.indexOf(entry);
                beginRemoveRows(QModelIndex(), index, index);
                it = m_list.erase(it);
                endRemoveRows();
                delete entry;
            } else {
                it++;
            }
        }
    }
}

void CategoriesModel::sortModel()
{
    beginResetModel();
    if (m_allCategory)
        std::sort(m_list.begin() + 1, m_list.end(), CategoryEntry::lessThan);
    else
        std::sort(m_list.begin(), m_list.end(), CategoryEntry::lessThan);
    endResetModel();
}

#include "moc_categoriesmodel.cpp"
