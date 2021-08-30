// SPDX-FileCopyrightText: 2018-2021 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef TASKSMODEL_H
#define TASKSMODEL_H

#include <QSortFilterProxyModel>
#include <QQmlComponent>

class Application;
class ApplicationsModel;

class TasksModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool hasMaximizedWindow READ hasMaximizedWindow NOTIFY hasMaximizedWindowChanged)
public:
    explicit TasksModel(QObject *parent = nullptr);

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    bool hasMaximizedWindow() const;

    Q_INVOKABLE Application *get(int row) const;
    Q_INVOKABLE int indexFromAppId(const QString &appId) const;

Q_SIGNALS:
    void hasMaximizedWindowChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;

private:
    ApplicationsModel *m_appsModel = nullptr;
};

QML_DECLARE_TYPE(TasksModel)

#endif // TASKSMODEL_H
