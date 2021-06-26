// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CATEGORIESMODEL_H
#define CATEGORIESMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlComponent>

class CategoryEntry;

class CategoriesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool includeAllCategory READ isAllCategoryIncluded WRITE includeAllCategory NOTIFY includeAllCategoryChanged)
public:
    enum Roles {
        NameRole = Qt::UserRole +1,
        CommentRole,
        IconNameRole,
        CategoryRole
    };
    Q_ENUM(Roles)

    CategoriesModel(QObject *parent = 0);
    ~CategoriesModel();

    bool isAllCategoryIncluded() const;
    void includeAllCategory(bool value);

    QHash<int, QByteArray> roleNames() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

Q_SIGNALS:
    void includeAllCategoryChanged();
    void refreshing();

private:
    bool m_initialized;
    QList<CategoryEntry *> m_list;
    bool m_allCategory;

    static void refresh(CategoriesModel *model);

private Q_SLOTS:
    void createAllCategory();
    void addEntry(CategoryEntry *entry);
    void cleanModel(const QStringList &list);
    void sortModel();
};

QML_DECLARE_TYPE(CategoriesModel)

#endif // CATEGORIESMODEL_H
