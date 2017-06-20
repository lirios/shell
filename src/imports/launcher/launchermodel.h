/****************************************************************************
 * This file is part of Liri Shell.
 *
 * Copyright (C) 2017 Pier Luigi Fiorini
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
