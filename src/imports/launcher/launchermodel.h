// SPDX-FileCopyrightText: 2018 Pier Luigi Fiorini
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LAUNCHERMODEL_H
#define LAUNCHERMODEL_H

#include <QtCore/QSortFilterProxyModel>
#include <QtQml/QQmlComponent>

class Application;
class ApplicationManager;

class LauncherModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit LauncherModel(QObject *parent = nullptr);

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    Q_INVOKABLE Application *get(int row) const;
    Q_INVOKABLE int indexFromAppId(const QString &appId) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const override;

private:
    ApplicationManager *m_appMan;
};

QML_DECLARE_TYPE(LauncherModel)

#endif // LAUNCHERMODEL_H
